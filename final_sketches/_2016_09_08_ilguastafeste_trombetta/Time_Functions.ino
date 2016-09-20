
void printTime()
{

  Serial.print(rtc.getHours());
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.print(rtc.getSeconds());
  Serial.println("");
  delay(1000);
}

boolean setTimeBool(int h, int m, int s)
{
  if(rtc.getHours() > h && rtc.getMinutes() > m && rtc.getSeconds() > s)
  {
    after = true;
  }
  return after;
}
