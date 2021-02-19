#ifndef FileService_h
#define FileService_h

class FileService {
  public:
    FileService();
    String readFile(String filename);
    bool writeFile(String filename, String content);
};

#endif