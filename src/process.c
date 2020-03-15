/*Pre-processor*/
/*potato-c's pre-processor job is to remove comments*/
/*replace tabs with spaces and other "cleanasing" stuff*/
void preProcess(int s, char * fd) {
	int commentLen;
	int i;
	int i2;
	/*Replace tabs with spaces*/
	for(i = 0; i < s; i++) {
		if(fd[i] == '\t') {
			fd[i] = ' ';
		}
	}
	/*Remove multi-line comments*/
	for(i = 0; i < s; i++) {
		if(memcmp(fd+i,token_list[COMMENT_OPEN],2) == 0) {
			commentLen = 0;
			while(memcmp(fd+i+commentLen,token_list[COMMENT_CLOSE],2) != 0) {
				commentLen++;
			}
			commentLen += 2; /*skip the closing comnment thing*/
			memmove(fd+i,fd+i+commentLen,s-i);
		}
	}
	/*Remove newlines (all of them, no exceptions)*/
	for(i = 0; i < s; i++) {
		if(fd[i] == '\n' && i2 == 0) {
			memmove(fd+i,fd+i+1,s-i);
		} else if(fd[i] == '\n' && i2 != 0) {
			i2 = 0;
		}
	}
	return;
}
