

//
// file IO
//

// returns the file size
// return -1 if file is not present
// the file should be in BINARY mode for stupid OSs that care
int Sys_FileOpenRead(char *path, int *hndl);

int Sys_FileOpenWrite(char *path);

int Sys_FileRead(int handle, void *dest, int count);
int Sys_FileWrite(int handle, void *data, int count);

void Sys_mkdir(char *path);


void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length);

//
// system IO
//
void Sys_DebugLog(char *file, char *fmt, ...);

void Sys_SendKeyEvents (void);
// Perform Key_Event () callbacks until the input que is empty

void Sys_LowFPPrecision(void);
void Sys_HighFPPrecision(void);

char *Sys_ConsoleInput (void);