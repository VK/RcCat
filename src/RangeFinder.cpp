
#include "RangeFinder.h"

#include "Timer.h"
#include "I2C.h"




#define VL6180x_FAILURE_RESET  -1

#define VL6180X_IDENTIFICATION_MODEL_ID              0x0000
#define VL6180X_IDENTIFICATION_MODEL_REV_MAJOR       0x0001
#define VL6180X_IDENTIFICATION_MODEL_REV_MINOR       0x0002
#define VL6180X_IDENTIFICATION_MODULE_REV_MAJOR      0x0003
#define VL6180X_IDENTIFICATION_MODULE_REV_MINOR      0x0004
#define VL6180X_IDENTIFICATION_DATE                  0x0006 //16bit value
#define VL6180X_IDENTIFICATION_TIME                  0x0008 //16bit value

#define VL6180X_SYSTEM_MODE_GPIO0                    0x0010
#define VL6180X_SYSTEM_MODE_GPIO1                    0x0011
#define VL6180X_SYSTEM_HISTORY_CTRL                  0x0012
#define VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO         0x0014
#define VL6180X_SYSTEM_INTERRUPT_CLEAR               0x0015
#define VL6180X_SYSTEM_FRESH_OUT_OF_RESET            0x0016
#define VL6180X_SYSTEM_GROUPED_PARAMETER_HOLD        0x0017

#define VL6180X_SYSRANGE_START                       0x0018
#define VL6180X_SYSRANGE_THRESH_HIGH                 0x0019
#define VL6180X_SYSRANGE_THRESH_LOW                  0x001A
#define VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD     0x001B
#define VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME        0x001C
#define VL6180X_SYSRANGE_CROSSTALK_COMPENSATION_RATE 0x001E
#define VL6180X_SYSRANGE_CROSSTALK_VALID_HEIGHT      0x0021
#define VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE  0x0022
#define VL6180X_SYSRANGE_PART_TO_PART_RANGE_OFFSET   0x0024
#define VL6180X_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT   0x0025
#define VL6180X_SYSRANGE_RANGE_IGNORE_THRESHOLD      0x0026
#define VL6180X_SYSRANGE_MAX_AMBIENT_LEVEL_MULT      0x002C
#define VL6180X_SYSRANGE_RANGE_CHECK_ENABLES         0x002D
#define VL6180X_SYSRANGE_VHV_RECALIBRATE             0x002E
#define VL6180X_SYSRANGE_VHV_REPEAT_RATE             0x0031

#define VL6180X_SYSALS_START                         0x0038
#define VL6180X_SYSALS_THRESH_HIGH                   0x003A
#define VL6180X_SYSALS_THRESH_LOW                    0x003C
#define VL6180X_SYSALS_INTERMEASUREMENT_PERIOD       0x003E
#define VL6180X_SYSALS_ANALOGUE_GAIN                 0x003F
#define VL6180X_SYSALS_INTEGRATION_PERIOD            0x0040

#define VL6180X_RESULT_RANGE_STATUS                  0x004D
#define VL6180X_RESULT_ALS_STATUS                    0x004E
#define VL6180X_RESULT_INTERRUPT_STATUS_GPIO         0x004F
#define VL6180X_RESULT_ALS_VAL                       0x0050
#define VL6180X_RESULT_HISTORY_BUFFER                0x0052
#define VL6180X_RESULT_RANGE_VAL                     0x0062
#define VL6180X_RESULT_RANGE_RAW                     0x0064
#define VL6180X_RESULT_RANGE_RETURN_RATE             0x0066
#define VL6180X_RESULT_RANGE_REFERENCE_RATE          0x0068
#define VL6180X_RESULT_RANGE_RETURN_SIGNAL_COUNT     0x006C
#define VL6180X_RESULT_RANGE_REFERENCE_SIGNAL_COUNT  0x0070
#define VL6180X_RESULT_RANGE_RETURN_AMB_COUNT        0x0074
#define VL6180X_RESULT_RANGE_REFERENCE_AMB_COUNT     0x0078
#define VL6180X_RESULT_RANGE_RETURN_CONV_TIME        0x007C
#define VL6180X_RESULT_RANGE_REFERENCE_CONV_TIME     0x0080

#define VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD      0x010A
#define VL6180X_FIRMWARE_BOOTUP                      0x0119
#define VL6180X_FIRMWARE_RESULT_SCALER               0x0120
#define VL6180X_I2C_SLAVE_DEVICE_ADDRESS             0x0212
#define VL6180X_INTERLEAVED_MODE_ENABLE              0x02A3





//macros to simplify i2c communicaiton

#define getRegister(registerAddr) i2c.readByteLongAddr(_i2caddress, registerAddr)
#define getRegister16bit(registerAddr) i2c.read2BytesLongAddr(_i2caddress, registerAddr)
#define setRegister(registerAddr, data) i2c.writeByteLongAddr(_i2caddress, registerAddr, data)
#define setRegister16bit(registerAddr, data) i2c.write2BytesLongAddr(_i2caddress, registerAddr, data)



#define MAX_SAME_DIST_TILL_RESTART 40

namespace RcCat
{

  RangeFinder rangeFinder;

  RangeFinder::RangeFinder()
  {
  }


  uint8_t  RangeFinder::internalSetup(){


    uint8_t data; //for temp data storage

    data = getRegister(VL6180X_SYSTEM_FRESH_OUT_OF_RESET);

    Serial.println(data);

    if(data != 1) return VL6180x_FAILURE_RESET;

    //Required by datasheet
    //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
    setRegister(0x0207, 0x01);
    setRegister(0x0208, 0x01);
    setRegister(0x0096, 0x00);
    setRegister(0x0097, 0xfd);
    setRegister(0x00e3, 0x00);
    setRegister(0x00e4, 0x04);
    setRegister(0x00e5, 0x02);
    setRegister(0x00e6, 0x01);
    setRegister(0x00e7, 0x03);
    setRegister(0x00f5, 0x02);
    setRegister(0x00d9, 0x05);
    setRegister(0x00db, 0xce);
    setRegister(0x00dc, 0x03);
    setRegister(0x00dd, 0xf8);
    setRegister(0x009f, 0x00);
    setRegister(0x00a3, 0x3c);
    setRegister(0x00b7, 0x00);
    setRegister(0x00bb, 0x3c);
    setRegister(0x00b2, 0x09);
    setRegister(0x00ca, 0x09);
    setRegister(0x0198, 0x01);
    setRegister(0x01b0, 0x17);
    setRegister(0x01ad, 0x00);
    setRegister(0x00ff, 0x05);
    setRegister(0x0100, 0x05);
    setRegister(0x0199, 0x05);
    setRegister(0x01a6, 0x1b);
    setRegister(0x01ac, 0x3e);
    setRegister(0x01a7, 0x1f);
    setRegister(0x0030, 0x00);



    //Recommended settings from datasheet
    //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf

    //Enable Interrupts on Conversion Complete (any source)
    setRegister(VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, (4 << 3)|(4) ); // Set GPIO1 high when sample complete


    setRegister(VL6180X_SYSTEM_MODE_GPIO1, 0x10); // Set GPIO1 high when sample complete
    setRegister(VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30); //Set Avg sample period
    setRegister(VL6180X_SYSALS_ANALOGUE_GAIN, 0x46); // Set the ALS gain
    setRegister(VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // Set auto calibration period (Max = 255)/(OFF = 0)
    setRegister(VL6180X_SYSALS_INTEGRATION_PERIOD, 0x63); // Set ALS integration time to 100ms
    setRegister(VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform a single temperature calibration
    //Optional settings from datasheet
    //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
    setRegister(VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x00); // Set default ranging inter-measurement period to 0x09 = 100ms  0x00 = 10ms
    setRegister(VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A); // Set default ALS inter-measurement period to 100ms
    setRegister(VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24); // Configures interrupt on ‘New Sample Ready threshold event’
    //Additional settings defaults from community
    setRegister(VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32);
    setRegister(VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);
    setRegister16bit(VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x7B );
    setRegister16bit(VL6180X_SYSALS_INTEGRATION_PERIOD, 0x64);

    setRegister(VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD,0x30);
    setRegister(VL6180X_SYSALS_ANALOGUE_GAIN,0x40);
    setRegister(VL6180X_FIRMWARE_RESULT_SCALER,0x01);


    delay(20);

    setRegister(VL6180X_SYSRANGE_START, 0x03);



    dist_not_changed = 0;
    last_timer  = 0;

    return 0;
  }






  void RangeFinder::setup(int i2caddress){
    _i2caddress = i2caddress; //set default address for communication

    internalSetup();
  }






  void RangeFinder::loop()
  {

    unsigned long get_timer = RcCat::timer.getCount();


    //if(get_timer  > last_timer + 100000l)
    if(get_timer  > last_timer + 10000l)
    {

      distance = getRegister(VL6180X_RESULT_RANGE_VAL);
      last_timer = get_timer;

      if(distance != old_distance)
      {
        dist_not_changed = 0;
      } else {
        dist_not_changed ++;
        if(dist_not_changed > MAX_SAME_DIST_TILL_RESTART && distance < 245)
        {
          Serial.println("error -> run setup again");
          internalSetup();
        }
      }
      old_distance = distance;

    }

  }

  uint8_t RangeFinder::getDistance()
  {
    return distance;
  }

}
