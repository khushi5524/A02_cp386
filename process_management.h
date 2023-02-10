const int SHMSIZE = 8192;
const char *SHMNAME = "/shm";
const char *FILE_NAME_OUTPUT = "output.txt";

int main(int argc, char *args[]);
void File_shm(char *file_ptr, char *flinenm) ;
void pipe_fd(int p_id);
void commandsopRun(char *varptr);
void filecmdwrite(char *buff_fd, int p_id);