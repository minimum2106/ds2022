const MAX_SIZE = 1024;
struct file{
    char filename[MAX_SIZE];
    char buffer[MAX_SIZE];
    int buffer_size;
};

program FILESTRANSFER {
	version FILESTRANSFER_1 {
		int transfer_file(file)=1;
	} = 1;
} = 0x31230000;