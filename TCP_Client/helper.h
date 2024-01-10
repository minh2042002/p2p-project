#ifndef HELPER_H
#define HELPER_H
int getID();
void getFileName(const char *path, char *filename);
void createFileIfNotExist(char *fileName);
int checkFileExistOrNot(char *filePath);
void saveFile(char *path, char *fileName);
void deleteFile(char *path);
long long getFileSize(char *filePath);
#endif