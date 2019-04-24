#ifndef TEMPERATURE_H
#define TEMPERATURE_H

// Determines frequency of data sampling
#define TEMP_TRACK_POINTS 800

struct tempTrack {
    double ssxx;            //squared x deviation for lin reg
    double ssxy;            //xy deviation for lin reg
    double meanVoltRead;    // mean volt reading of records
    double meanTime;        //mean time reading of recors
    double approxTemp;
    uint16_t digVoltReads[TEMP_TRACK_POINTS];  //volt reading record
    unsigned long readTime[TEMP_TRACK_POINTS];   //time reading recors
    unsigned int tempWindow;// Window to use for temperature readings
    unsigned int dispTemp; // Current displayed temperature
    unsigned int readTrack;        //keeps track of oldest temp reading
    unsigned long prevTempRead; //time last measurements was taken
};

tempTrack *temp;

void initTempTrack(tempTrack* iniTemp){
  iniTemp->tempWindow = 2000;
  for (int i = 0; i < TEMP_TRACK_POINTS; i++) {
    iniTemp->digVoltReads[i] = 0;
    iniTemp->readTime[i] = 0;
  } 
  iniTemp->ssxx = 0;
  iniTemp->ssxy = 0;
  iniTemp->meanVoltRead = 0;
  iniTemp->meanTime = 0;
  iniTemp->approxTemp = 0;
  iniTemp->readTrack = 0;        
  iniTemp->prevTempRead = 0;
  temp = iniTemp;
}

// updMean computes the new mean Volt and Temp readings 
// and updates meanVoltRead, meanTime
void updMean(uint16_t newVolt, double newMilli, tempTrack* Temp) { 
  // Compute difference between current and oldest reading
  double deltaVolt = ((int) newVolt - (int) Temp->digVoltReads[Temp->readTrack]); 
  double deltaTime = (newMilli - Temp->readTime[Temp->readTrack]);

  // Update mean variables
  Temp->meanVoltRead += deltaVolt/((double)TEMP_TRACK_POINTS);
  Temp->meanTime     += deltaTime/((double) TEMP_TRACK_POINTS);

  // Replace oldest measurement with current
  Temp->digVoltReads[Temp->readTrack] = newVolt;
  Temp->readTime[Temp->readTrack]     = newMilli;

  // Update read tracker, check for loops
  Temp->readTrack = (Temp->readTrack + 1) % TEMP_TRACK_POINTS;
}

// updates the linear regression variables ssxx, ssxy, Temp.meanVoltRead, Temp.meanTime
// and Temp.digVoltReads[]. 
/* 
void updLinReg(uint16_t newVolt, double newMilli) {
  Temp.meanVoltRead = Temp.meanVoltRead + 1.0*(newVolt - Temp.digVoltReads[0]) / TEMP_TRACK_POINTS;
  Temp.meanTime = Temp.meanTime + (newMilli - Temp.readTime[0]) / TEMP_TRACK_POINTS;

  Temp.ssxx = Temp.ssxx - (Temp.readTime[0] * Temp.readTime[0] - TEMP_TRACK_POINTS * Temp.meanTime * Temp.meanTime);
  Temp.ssxy = Temp.ssxy - (Temp.digVoltReads[0] * Temp.readTime[0] - 
                     TEMP_TRACK_POINTS * Temp.meanVoltRead * Temp.meanTime);

  Temp.ssxx = Temp.ssxx + newMilli * newMilli - 
                     TEMP_TRACK_POINTS * Temp.meanTime * Temp.meanTime;
  Temp.ssxy = Temp.ssxy + newVolt * newMilli - 
                     TEMP_TRACK_POINTS * Temp.meanTime * Temp.meanVoltRead;

  for (int i = 0; i < TEMP_TRACK_POINTS - 1; i++) {
    Temp.readTime[i] = Temp.readTime[i+1];
    Temp.digVoltReads[i] = Temp.digVoltReads[i+1];
  }

  Temp.readTime[TEMP_TRACK_POINTS - 1] = newMilli;
  Temp.digVoltReads[TEMP_TRACK_POINTS - 1] = newVolt;
}
*/


// compute temperature with linear regression and write to Temp
void updateTemp(tempTrack* Temp) {
  double b0;
  double b1;
  uint16_t tempRead = analogRead(TEMP_PIN);
  updMean(tempRead, millis(), Temp);
  // code to use the updLinReg function for linear regression
  // disabled for now since idk how updLinReg works or where I 
  // got the algo from
  /*
  updLinReg(tempRead, millis());
  b1 = Temp.ssxy / Temp.ssxx;
  b0 = Temp.meanVoltRead - b/1 * Temp.meanTime;
  // 98 = voltage to temp conversion factor for ad595,
  // 5/1023, conversion from voltage read to mV
  Temp.approxTemp = (b1 * millis() + b0)* (5.0/1023.0)* 98;
  */
  Temp->approxTemp = 98 * (5.0/1023.0) * Temp->meanVoltRead;
}


#endif
