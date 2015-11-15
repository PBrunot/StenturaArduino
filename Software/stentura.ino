#define ACTION_RESET 0x14
#define ACTION_OPEN 0x0A
#define ACTION_READC 0x0B
#define TIMEOUT 500
#define BUFFER_SIZE 256

struct StenturaData
{
  byte* data;
  unsigned int checksum;
  bool dataValid;
};

struct StenturaResponse
{
  byte SOH; // B0
  byte seq; // B1
  unsigned int len; // B2-3
  unsigned int action; // B4-5
  unsigned int error; // B6-7
  unsigned int p1; // B8-9
  unsigned int p2; // B10-11
  unsigned int checksum; // B12-13
  StenturaData* dataSection; // B14
};

struct StenturaRequest
{
  byte SOH;
  byte seq;
  unsigned int len;
  unsigned int action;
  unsigned int error;
  unsigned int p1;
  unsigned int p2;
  unsigned int p3;
  unsigned int p4;
  unsigned int p5;
  unsigned int checksum;
  StenturaData* dataSection;
  bool headerComplete;
  bool headerValid;
  bool dataComplete;
};


byte* bufferIn;
byte* buffer2;
byte* bufferOut;

byte bufferInLen = 0;
byte bufferOutLen = 0;
unsigned long receivedMillis = 0;

StenturaRequest pendingRequest;
StenturaData pendingRequestData;

void ParseRequest(byte* rawData, int len)
{
  if (len <= 0) return;
  if (rawData == NULL) return;
  int pos = 0;
  
  StenturaRequest* req = &pendingRequest;
  while (pos < len)
  {
    switch (pos)
    {
      case 0:
        req->SOH = rawData[pos];
        pos++;
        break;
      case 1:
        req->seq = rawData[pos];
        pos++;
        break;
      case 2:
        req->len = rawData[pos];
        pos++;
        break;
      case 3:
        req->len += rawData[pos] * 256;
        pos++;
        break;
      case 4:
        req->action = rawData[pos];
        pos++;
        break;
      case 5:
        req->action += rawData[pos] * 256;
        pos++;
        break;
      case 6:
        req->error = rawData[pos];
        pos++;
        break;
      case 7:
        req->error += rawData[pos] * 256;
        pos++;
        break;
      case 8:
        req->p1 = rawData[pos];
        pos++;
        break;
      case 9:
        req->p1 += rawData[pos] * 256;
        pos++;
        break;
      case 10:
        req->p2 = rawData[pos];
        pos++;
        break;
      case 11:
        req->p2 += rawData[pos] * 256;
        pos++;
        break;
      case 12:
        req->p3 = rawData[pos];
        pos++;
        break;
      case 13:
        req->p3 += rawData[pos] * 256;
        pos++;
        break;
      case 14:
        req->p4 = rawData[pos];
        pos++;
        break;
      case 15:
        req->p4 += rawData[pos] * 256;
        pos++;
        break;
      case 16:
        req->p5 = rawData[pos];
        pos++;
        break;
      case 17:
        req->p5 += rawData[pos] * 256;
        pos++;
        break;
      case 18:
        req->checksum = rawData[pos];
        pos++;
        break;
      case 19:
        req->checksum += rawData[pos] * 256;
        pos++;
        req->headerComplete = true;
        break;
    }
  }

  if (req->headerComplete && !req->headerValid)
  {
    req->headerValid = checkCrc(req);
  }
  if (len > 22)
  {
    req->dataSection->data = &rawData[20];
    req->dataSection->checksum = rawData[len-2] + rawData[len-1] * 256;
    req->dataSection->dataValid = checkCrc(req->dataSection);
  }
  
}

bool checkCrc(StenturaRequest* req)
{
  return true;
}

bool checkCrc(StenturaData* data)
{
  return true;
}

void setup() {
  // to avoid dynamic allocations static buffer are used
  bufferIn = new byte[BUFFER_SIZE];
  buffer2 = new byte[BUFFER_SIZE];
  bufferOut = new byte[BUFFER_SIZE];
  memset(bufferIn, BUFFER_SIZE, 0);
  memset(buffer2, BUFFER_SIZE, 0);
  memset(bufferOut, BUFFER_SIZE, 0);
  memset(&pendingRequest, sizeof(pendingRequest), 0);
  memset(&pendingRequestData, sizeof(pendingRequestData), 0);
  
  // state init
  pendingRequest.dataSection = &pendingRequestData;
  bufferInLen = 0;
  bufferOutLen = 0;
  receivedMillis = millis();

  // Open serial port
  Serial.begin(9600);
  while (!Serial);
}

void loop() {
  unsigned long currentTime = millis();

  // INCOMING REQUESTS PROCESSING
  if (currentTime - receivedMillis < TIMEOUT)
  {
    while (Serial.available() && bufferInLen < BUFFER_SIZE)
    {
      bufferIn[bufferInLen] = Serial.read();
      bufferIn++;
      receivedMillis = millis();
    }
  }
  else
  {
    // NOTHING RECEIVED FOR TIMEOUT MS -> LET'S PROCESS THE BUFFER.
    if(bufferIn > 0)
    {
      // Parse the incoming buffer into pendingRequest
      ParseRequest(bufferIn, bufferInLen);

      // Prepare answer in bufferOut
      for(int i=0;i<=7;i++) bufferOut[i] = bufferIn[i];
      bufferOut[8]=0;
      bufferOut[9]=0;
      bufferOut[10]=0;
      bufferOut[11]=0;
      
      bufferOutLen = 12;

      // Transmit bufferOut to Serial
      Serial.write(bufferOut, bufferOutLen);
  
      // Reset incoming data
      memset(&pendingRequest, sizeof(pendingRequest), 0);
      pendingRequest.dataSection = &pendingRequestData;
      memset(&pendingRequestData, sizeof(pendingRequestData), 0);
      bufferInLen = 0;

      // Reset output buffers since we have data to process
      memset(bufferOut, BUFFER_SIZE, 0);
      bufferOutLen = 0;
    }
  }
  
}
