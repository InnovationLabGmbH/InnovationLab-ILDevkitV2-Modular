#ifndef APPLICATION_H
#define APPLICATION_H

#include "Led.h"
#include "ADG732.h"
#include "ADS7049.h"
#include "MAX14661.h"
#include "MCP47X6.h"
#include "NanoBLEFlashPrefs.h"
#include "filterInterface.h"
#include "byPass.h"
#include "movAverage.h"
#include "movAverageCumulative.h"
#include "movAverageWeighted.h"
#include "median.h"
#include "kalmanFilter.h"

#define HARDWARE_VER_1 0
#define FIRMWARE_VER_1 5
#define FIRMWARE_VER_2 1
#define FIRMWARE_VER_3 0

#define VERSION (((uint32_t)(HARDWARE_VER_1) << 24) | \
                 ((uint32_t)(FIRMWARE_VER_1) << 16) | \
                 ((uint32_t)(FIRMWARE_VER_2) << 8) |  \
                 ((uint32_t)(FIRMWARE_VER_3) << 0))

#define MAX_X 32U
#define MAX_Y 32U

#define DATA_RESOLUTION 12U
#define DATA_TYPE uint16_t
#define FILTER_DATA_TYPE DATA_TYPE

#define COMM_ID_EMPTY 0x00
#define COMM_ID_START 0x01
#define COMM_ID_STOP 0x02
#define COMM_ID_START_CAN 0x03
#define COMM_ID_FRAME 0x04
#define COMM_ID_STOP_CAN 0x05
#define COMM_ID_WRITE_CONFIG 0x08
#define COMM_ID_READ_CONFIG 0x09
#define COMM_ID_READ_FIRMWARE_VERSION 0x0A
#define COMM_ID_START_NO_PARAMS 0x0B
#define COMM_ID_WRITE_FLTR_CONF 0x12
#define COMM_ID_READ_FLTR_CONF 0x13

//REQUEST STATUS
#define SESSION_STARTED 0x00
#define SESSION_STOPED 0x01

//FILTER TYPES
#define    FILTER_NONE                            0U
#define    FILTER_MOV_AVERAGE                     1U
#define    FILTER_MOV_AVERAGE_CUMULATIVE          2U
#define    FILTER_MOV_AVERAGE_WEIGHTED            3U
#define    FILTER_MEDIAN                          4U
#define    FILTER_KALMAN                          5U

#define MOV_AVERAGE_MAX_WINDOW_SIZE              20U
#define MOV_AVERAGE_CUMMULATIVE_MAX_COEF         99U
#define MOV_AVERAGE_WEIGHTED_MAX_WINDOW_SIZE     20U
#define MEDIAN_MAX_WINDOW_SIZE                   20U
#define KALMAN_MAX_ERROR                         99U
#define KALMAN_MAX_SPEED                        100U







struct ScanSettings
{
  uint16_t m_MatrixShiftX;
  uint16_t m_MatrixShiftY;
  uint8_t m_xSize;
  uint8_t m_ySize;
  uint8_t m_samplesNumber;
  uint8_t m_frameRate;
  uint8_t m_adcDelay;
  uint8_t m_refferenceVoltage;
  uint8_t m_filterType;
  //Moving average filter params
  uint16_t m_movAvrWindowSize;
  //Moving average cumulative filter params
  uint16_t m_movAverCumulativeFiltrationCoef;
  //Moving average weighted filter params
  uint16_t m_movAverWeightedWindowSize;
  //Median filter params
  uint16_t m_medianWindowSize;
  //Kalman filter params
  uint16_t m_kalmanErrMeasure;
  uint16_t m_kalmanMeasureSpeed;

  ScanSettings();
  ~ScanSettings();
  void reset();
};

struct Command
{
  uint8_t m_code;
  uint8_t *m_pData;
  uint16_t m_dataSize;

  Command();
  ~Command();
  bool reset();
};

class Application
{
public:
  Application();
  ~Application();
  bool init(Led *l_pLed, uint32_t l_rxBufferSize, uint32_t l_txBufferSize, int16_t l_analogEnablePin, int16_t l_internalDacPin, int16_t l_externalTriggerPin, MCP47X6 *l_pDac, MAX14661 *l_pMuxDriveLines, admux::Mux *l_pDeMuxScanLines, Ads7049 *l_pAdc);
  volatile bool scan();
  bool checkCommand();
  bool getCommand(Command &l_command);

private:
  int16_t m_analogEnablePin;

  Led* m_pLed;

  int16_t m_internalDacPin;
  int16_t m_externalTriggerPin;
  int16_t m_externalTriggerFlag;

  MCP47X6 *m_pDac; //MCP4716
  MAX14661 *m_pMuxDriveLines; //MAX14661
  admux::Mux *m_pDeMuxScanLines; //ADG732
  Ads7049 *m_pAdc; //ADS7049

  bool m_scanning : 1, m_doStop : 1;

  uint8_t *m_pRxBuffer;
  uint32_t m_rxBufferSize;
  uint32_t m_rxIndex;

  uint8_t *m_pTxBuffer;
  uint32_t m_txBufferSize;
  uint32_t m_txLength;

  ScanSettings m_scanSettings;
  uint32_t m_UnixTime;

  NanoBLEFlashPrefs myFlashPrefs;

  Interface::IFilter* m_pFilter;
  uint16_t m_filterWindowSize;

  bool writeSettings();
  bool readSettings();

  bool checkExtTriggerStart();
  bool checkExtTriggerStop();

  bool checkPreamble(uint8_t *l_pData);
  bool resetAnswer();
  bool pushHeaderToAnswer();
  bool pushToAnswer(uint8_t l_data);
  bool sendAnswer();
  bool sendAnswerCRC();

  bool setReferenceVoltage(uint16_t l_refVoltage);
  bool doSample(uint16_t *buffer, uint32_t rowStart, uint32_t rowCount);
  bool sendFrame(DATA_TYPE*l_pData, uint32_t l_dataLength, uint32_t l_packageId, uint32_t l_timestamp, uint32_t l_unixTime);
};
#endif
