from dash.dependencies import Input, Output, State
import dash
import dash_core_components as dcc
import dash_html_components as html
import plotly.graph_objs as go
import plotly.express as px
from plotly.subplots import make_subplots
import plotly.graph_objects as go


# handle data inport
import serial
import io
import threading
import time
import pandas as pd


class RcCatSerialIO:

    def __init__(self):
        self.ser = None
        self.readData = pd.DataFrame(columns=['time',
                                              'steer_rec', 'acc_rec',
                                              'ax', 'ay', 'az', 'a_tot',
                                              'pitch', 'roll',
                                              'dist',
                                              'speed',
                                              'steer_out',
                                              'acc_out',
                                              'state'
                                              ])
        self.thread = None
        self.active = False
        self.lastcall = pd.Timestamp.now()

    def connect(self, deviceName="/dev/ttyUSB0"):
        try:
            self.ser = serial.Serial(deviceName, 115200, timeout=0.0001)
            self.thread = threading.Thread(target=self.readSerial)
            self.thread.daemon = True
            self.active = True
            self.thread.start()

            return ""
        except:
            self.active = False
            return "No connection"

    def disconnect(self):
        self.active = False

        try:
            if self.ser:
                self.ser.close()
        except:
            print("Serial close problem")

    def readSerial(self):

        while self.active:
            try:
                l = self.ser.readline().decode("utf-8")

                if len(l) > 30:
                    strings = l.replace("\r\n", "").split("\t")
                    vals = [int(s) for s in strings]
                    if len(vals)+1 == len(self.readData.columns):
                        self.readData.loc[len(self.readData)] = [
                            pd.Timestamp.now(), *vals]
                else:
                    time.sleep(0.1)

            except:
                time.sleep(0.1)
                pass

    def status(self):
        return self.active


rcCatSerialIO = RcCatSerialIO()


# end handle data import

# create dash app

# external CSS stylesheets
external_stylesheets = [
    'https://codepen.io/chriddyp/pen/bWLwgP.css',
    {
        'href': 'https://stackpath.bootstrapcdn.com/bootswatch/4.4.1/sketchy/bootstrap.min.css',
        'rel': 'stylesheet',
        'integrity': 'sha384-2kOE+STGAkgemIkUbGtoZ8dJLqfvJ/xzRnimSkQN7viOfwRvWseF7lqcuNXmjwrL',
        'crossorigin': 'anonymous'
    }
]
app = dash.Dash("RcCat Monitor", external_stylesheets=external_stylesheets)


app.layout = html.Div([
    html.H1('RcCat Monitor'),

    html.P('Connect to your RcCat via a serial connection.'),
    html.Div([
        dcc.Input(
            placeholder='USB tty',
            type='text',
            value='/dev/ttyUSB0',
            id='connect-path'
        ),
        html.Button('Connect', id='connect-button',
                    className="btn btn-success ml-1", disabled=False),
        html.Button('Disconnect', id='disconnect-button',
                    className="btn btn-danger ml-1", disabled=True),
        html.Label('', id='connect-status', className="ml-3")
    ], className="input-group, p-1"),

    html.Div([

        html.Button('Refresh', id='refresh-button',
                    className="btn btn-primary ml-1"),

        dcc.Checklist(
            options=[
                {'label': 'Auto Refresh', 'value': 'ON'},
            ],
            value=[],
            labelStyle={'display': 'inline-block'},
            id='auto-refresh',
            className="pl-5 mr-5 mt-1"
        ),

        html.Label("Limit:", className="mt-1"),
        dcc.Input(
            id="time-limit",
            type="number",
            value=10000
        )

    ], className="input-group p-1"),


    dcc.Interval(
        id='interval-component',
        interval=1000,  # in milliseconds
        n_intervals=0
    ),


    dcc.Graph(id='main-graph')
], className="container")


# connect callback
@app.callback([
    Output('connect-button', 'disabled'),
    Output('disconnect-button', 'disabled'),
    Output('connect-status', 'children'),
],
    [
    Input('connect-button', 'n_clicks'),
    Input('disconnect-button', 'n_clicks'),
],
    [
        State('connect-path', 'value')
]
)
def connect_and_disconnect(connect_clicks, disconnect_clicks, connect_path_value):

    ctx = dash.callback_context

    if ctx.triggered and ctx.triggered[0]['value'] is not None:
        triggerID = ctx.triggered[0]['prop_id'].split('.')[0]

        print(triggerID)
        if triggerID == "connect-button":

            res = rcCatSerialIO.connect(connect_path_value)

            if res == "":
                return True, False, "connected"
            else:
                return False, True, res

        if triggerID == "disconnect-button":
            rcCatSerialIO.disconnect()
            return False, True, "disconnected"

    if rcCatSerialIO.status:
        return True, False, "connected"
    else:
        return False, True, "disconnected"


PLOT_LAYOUT = dict(
    margin={'l': 40, 'b': 30, 't': 10, 'r': 0},
    font=dict(
        family='Neucha',
        size=15,
        color='#555'
    ),
)


@app.callback(Output('interval-component', 'disabled'),
              [Input('auto-refresh', 'value')
               ])
def toggle_auto_refresh(values):
    if "ON" in values:
        return False
    else:
        return True


@app.callback(Output('main-graph', 'figure'),
              [Input('interval-component', 'n_intervals'),
               Input('refresh-button', 'n_clicks'),
               Input('time-limit', 'value'),
               ])
def update_graph(n, c, limit):

    if rcCatSerialIO.status():

        data = rcCatSerialIO.readData.copy()

        fig = make_subplots(
            rows=4, cols=1, shared_xaxes=True, vertical_spacing=0.02)

        fig.add_trace(go.Scatter(
            y=data["steer_rec"][-limit:], mode="lines", name="Steering Receiver"), row=1, col=1)
        fig.add_trace(go.Scatter(
            y=data["acc_rec"][-limit:], mode="lines", name="Acceleration Receiver"), row=1, col=1)
        fig.add_trace(go.Scatter(
            y=data["steer_out"][-limit:], mode="lines", name="Steering Output"), row=1, col=1)
        fig.add_trace(go.Scatter(
            y=data["acc_out"][-limit:], mode="lines", name="Acceleration Output"), row=1, col=1)

        fig.add_trace(go.Scatter(
            y=data["ax"][-limit:], mode="lines", name="gx"), row=2, col=1)
        fig.add_trace(go.Scatter(
            y=data["ay"][-limit:], mode="lines", name="gy"), row=2, col=1)
        fig.add_trace(go.Scatter(
            y=data["az"][-limit:], mode="lines", name="gz"), row=2, col=1)
        fig.add_trace(go.Scatter(
            y=data["a_tot"][-limit:], mode="lines", name="gtot"), row=2, col=1)
        fig.add_trace(go.Scatter(
            y=data["dist"][-limit:], mode="lines", name="Ground Dist"), row=2, col=1)

        fig.add_trace(go.Scatter(
            y=data["pitch"][-limit:], mode="lines", name="Pitch"), row=3, col=1)
        fig.add_trace(go.Scatter(
            y=data["roll"][-limit:], mode="lines", name="Roll"), row=3, col=1)

        fig.add_trace(go.Scatter(
            y=data["speed"][-limit:], mode="lines", name="Speed"), row=4, col=1)
        fig.add_trace(go.Scatter(
            y=data["state"][-limit:], mode="lines", name="State"), row=4, col=1)

        fig["layout"]["height"] = 800
        fig["layout"].update(**PLOT_LAYOUT)
        return fig
    else:

        return px.scatter(x=[0], y=[0])

        # end dash app
if __name__ == '__main__':
    app.run_server()
