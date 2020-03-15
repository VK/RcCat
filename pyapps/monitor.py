from dash.dependencies import Input, Output, State
import dash
import dash_core_components as dcc
import dash_html_components as html
import dash_daq as daq
import math

import plotly.graph_objs as go
import plotly.express as px
from plotly.subplots import make_subplots
import plotly.graph_objects as go
import pandas as pd
import base64
import urllib

import rccat


rcCatSerialIO = rccat.SerialIO()


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
    html.H1('RcCat Control'),

    html.Div(className="card", children=[
        dcc.Tabs(id="main-tabs", children=[
            dcc.Tab(label='Connect', className="card-header", children=[
                html.Div(className="card-body", children=[
                    html.P(
                        'Connect to your RcCat via a serial connection (e.g. /dev/ttyUSB0) or via WiFi relais (e.g. http://rccat.local).'),
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
                ])
            ]),
            dcc.Tab(label='Control', className="card-header",
                    children=[html.Div(className="card-body", children=[
                        html.P('Use the joystick to remote control your RcCat.'),
                        daq.Joystick(
                            id='joystick',
                            size=250,
                            className="d-flex justify-content-center"
                        ),
                        html.Label('', id="joystick-label",
                                   style="visible:hidden"),

                        html.P(
                            'Change internal parematers to play with driving characteristics:',
                            className="mt-3"),


                        html.Div([
                                 html.Label("Parameter ID: ",
                                            className="mr-1 mt-1"),
                                 dcc.Input(
                                     id="send-parameter-id",
                                     type="number",
                                     value=1
                                 ),
                                 html.Label(
                                     "Value: ", className="mr-1 ml-4 mt-1"),
                                 dcc.Input(
                                     id="send-parameter-value",
                                     type="number",
                                     value=100
                                 ),
                                 html.Button('Send', id='send-parameter-button',
                                             className="btn btn-primary ml-4"),

                                 ], className="input-group p-1"),

                        html.Label('', id="send-parameter-output",
                                   style="visible:hidden"),


                    ])]),
            dcc.Tab(label='Save/Load', className="card-header",
                    children=[html.Div(className="card-body", children=[

                        html.P(
                            "Upload old RcCat log files for data analysis, or download current data."),

                        dcc.Upload(
                            id='upload-file',
                            children=html.Div([
                                'Drag and Drop or ',
                                html.A('Select a RcCat log file for upload.')
                            ]),
                            style={
                                'vw': '100%',
                                'height': '60px',
                                'lineHeight': '60px',
                                'borderWidth': '1px',
                                'borderStyle': 'dashed',
                                'borderRadius': '5px',
                                'textAlign': 'center',
                                'margin': '10px'
                            },
                        ),
                        html.Label('', id="file-label",
                                   style="visible:hidden"),
                        html.Button('Create Download', id='download-button',
                                    className="btn btn-primary ml-1"),
                        html.A(
                            'Download Data',
                            id='download-link',
                            download="rawdata.txt",
                            href="",
                            target="_blank",
                            hidden=True,
                            className="ml-3"
                        )



                    ])]),
            dcc.Tab(label='Telemetry', className="card-header",
                    children=[html.Div(className="card-body", children=[
                        html.P('Check the internal telemetry data of the RcCat.'),

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

                            html.Label("Frames: ", className="mt-1"),
                            dcc.Input(
                                id="time-limit",
                                type="number",
                                value=10000
                            ),
                            html.Button('Clear', id='clear-button',
                                        className="btn btn-primary ml-1"),

                        ], className="input-group p-1"),

                        dcc.Graph(id='main-graph'),

                        dcc.Interval(
                            id='interval-component',
                            interval=1000,  # in milliseconds
                            n_intervals=0
                        ),


                    ])]),

        ]),
    ]),







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

    if rcCatSerialIO.status():
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


index = 0


@app.callback(Output('interval-component', 'disabled'),
              [Input('auto-refresh', 'value')
               ])
def toggle_auto_refresh(values):
    if "ON" in values:
        return False
    else:
        return True


@app.callback(Output('joystick-label', 'children'),
              [Input('joystick', 'angle'),
               Input('joystick', 'force')
               ])
def update_joystick(angle, force):

    if(isinstance(angle, (int, float)) and isinstance(force, (int, float))):
        throttle = math.sin(angle/180.0*math.pi)*force*100.0
        steer = math.cos(angle/180.0*math.pi)*force*100.0

        rcCatSerialIO.driveCommand(steer, throttle)

        return "{} {}".format(throttle, steer)
    else:
        return ""


@app.callback(Output('send-parameter-output', 'children'),
              [Input('send-parameter-button', 'n_clicks')],
              [State('send-parameter-id', 'value'),
               State('send-parameter-value', 'value')
               ])
def send_parameter(clicks, param_id, param_value):

    if clicks > 0 and (isinstance(param_id, int) and isinstance(param_value, int)):

        rcCatSerialIO.setParam(param_id, param_value)

        return "{} {}".format(param_id, param_value)
    else:
        return ""


@app.callback(Output('main-graph', 'figure'),
              [Input('interval-component', 'n_intervals'),
               Input('refresh-button', 'n_clicks'),
               Input('time-limit', 'value'),
               Input('clear-button', 'n_clicks'),
               ])
def update_graph(n, c, limit, clear_clicks):

    ctx = dash.callback_context

    if ctx.triggered and ctx.triggered[0]['value'] is not None:
        triggerID = ctx.triggered[0]['prop_id'].split('.')[0]
        print(triggerID)
        if triggerID == "clear-button":
            rcCatSerialIO.clearBuffer()

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


@app.callback(
    [
        Output('file-label', 'children')
    ],
    [
        Input('upload-file', 'contents')
    ],
)
def upload_file(file_data):

    _, content_string = file_data.split(',')
    decoded = base64.b64decode(content_string)

    rcCatSerialIO.importData(decoded)

    return ["test"]


@app.callback([Output('download-link', 'hidden'),
               Output('download-link', 'href')
               ],
              [Input('download-button', 'n_clicks')
               ])
def download_file(download_clicks):

    res = rcCatSerialIO.getDataString()
    data_string = "data:text/tsv;charset=utf-8," + urllib.parse.quote(res)
    return len(res) == 0, data_string

    # end dash app
if __name__ == '__main__':
    app.run_server(host="0.0.0.0")
