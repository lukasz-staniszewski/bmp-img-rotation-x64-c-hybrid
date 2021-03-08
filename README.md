#BMP IMAGE ROTATION WITH C/x86-64 HYBRID PROGRAMMING

#### ŁUKASZ STANISZEWSKI

### I. HOW TO:
+ ADD .BMP (BLACK-WHITE) IMAGE FILE TO MAIN DIRECTORY
+ CHANGE FILE NAME IN <i><b>project.c</b></i> FILE:
```
	int main(int argc, char* argv[])
	{
		imgInfo* pInfo;
		if (sizeof(bmpHdr) != 62)
		{
			printf("Change compilation options so as bmpHdr struct size is 62 bytes.\n");
			return 1;
		}
		if((pInfo = readBMP("eiti.bmp"))!=0)			// <---- HERE
		{
			allocate_for_output(pInfo);								
			change_headder(pInfo);
			...
```
+ THROUGH SYSTEM CONSOLE CHANGE DIRECTORY TO PROJECT'S DIR AND TYPE:
```make all```
+ RESULT IMAGE IS IN <i><b>result.bmp</b></i> FILE