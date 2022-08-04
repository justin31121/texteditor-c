#include "./js.h"

int string_chop_int64_t(string *s, int64_t *n) {
	size_t i=0;
	int64_t sum = 0;
	int negative = 0;
	if(s->count && s->data[0]=='-') {
		negative = 1;
		i++;
	}
	while(i<s->count && '0' <= s->data[i] && s->data[i] <= '9') {
		sum*=10;
		int digit = (s->data[i] - '0');
		sum+=digit;
		i++;
	}

	s->data+=i;
	s->count-=i;

	if(negative) sum*=-1;
	if(n) *n = sum;

	return i>0;
}

int string_chop_int(string *s, int *n) {
	size_t i=0;
	int sum = 0;
	int negative = 0;
	if(s->count && s->data[0]=='-') {
		negative = 1;
		i++;
	}
	while(i<s->count && '0' <= s->data[i] && s->data[i] <= '9') {
		sum*=10;
		int digit = (s->data[i] - '0');
		sum+=digit;
		i++;
	}

	s->data+=i;
	s->count-=i;

	if(negative) sum*=-1;
	if(n) *n = sum;

	return i>0;
}

char* slurp_file(const char* file_path, size_t *size) {
	FILE *f=fopen(file_path, "rb");
	if(!f) {
		fprintf(stderr, "ERROR: Can not open file '%s' :%s", file_path, strerror(errno));
		exit(1);
	}
	if(fseek(f, 0,SEEK_END)<0) {
		fprintf(stderr, "ERROR: Can not read file '%s' :%s", file_path, strerror(errno));
		exit(1);
	}	
	long m = ftell(f);
	if(m<0) {
		fprintf(stderr, "ERROR: Can not read file '%s' :%s", file_path, strerror(errno));
		exit(1);
	}
	char* buffer = malloc(sizeof(char)*m);
	if(!buffer) {
		fprintf(stderr, "ERROR: Can not allocate memory\n");
		exit(1);
	}
	
	if(fseek(f, 0, SEEK_SET)<0) {
		fprintf(stderr, "ERROR: Can not read file '%s' :%s", file_path, strerror(errno));
		exit(1);
	}

	size_t n = fread(buffer, 1, m, f);
	buffer[m]='\0';
	assert((int) n==m);

	if(f) {
		if(fclose(f)!=0) {
			fprintf(stderr, "ERROR: Can when closing the file '%s' :%s", file_path, strerror(errno));
			exit(1);
		}
	}
	if(size) (*size)=n;
	return buffer;
}

string string_from_parts(const char* data, size_t count) {
	string s;
	s.count = count;
	s.data=data;
	return s;
}

string string_from_cstr(const char* data) {
	return string_from_parts(data, strlen(data));
}

string string_trim_left(string s) {
	size_t i = 0;
	while(i<s.count && isspace(s.data[i])) {
		i++;
	}
	return string_from_parts(s.data+i, s.count-i);
}

string string_trim_right(string s) {
	size_t i = 0;
	while(i<s.count && isspace(s.data[s.count - 1 - i])) {
		i++;
	}
	return string_from_parts(s.data, s.count-i);
}

string string_trim(string s) {
	return string_trim_right(string_trim_left(s));
}

void string_in_cstr(string s, char* target) {
	for(size_t i=0;i<s.count;i++) {
		target[i]=s.data[i];
	}
	target[s.count]='\0';
}

char* string_to_cstr(string *s) {
	char* res = (char *) malloc( s->count * sizeof(char) );
	
	for(size_t i=0;i<s->count;i++) {
		res[i]=s->data[i];
	}
	res[s->count]='\0';
	return res;
}

string string_chop_by_delim(string *s, char delim) {
	size_t i = 0;
	while(i < s->count && s->data[i]!=delim) {
		i+=1;
	}
	
	string result = string_from_parts(s->data, i);
	
	if(i < s->count) {
		s->count -= i+1;
		s->data += i+1;
	}
	else {
		s->count -= i;
		s->data  += i;
	}

	return result;
}

int string_to_int(string s, int *target) {
	char s1[s.count];
	string_in_cstr(s, s1);
	long lnum;
	int num;
	char* end;

	errno = 0;
	lnum = strtol(s1, &end, 10);
	if(end==s1) {
		//fprintf(stderr, "ERROR: Can't convert string to number\n");
		return 0;
	}

	if((lnum == LONG_MAX || lnum == LONG_MIN) && errno == ERANGE) {
		// fprintf(stderr, "ERROR: number out of range for LONG\n");
		return 0;
	}
	
	if((lnum > INT_MAX) || (lnum < INT_MIN) ) {
		//fprintf(stderr, "ERROR: number out of range for INT\n");
		return 0;
	}
	
	num = (int) lnum;
	if(target) (*target) = num;
	return 1;
}

int char_to_int(char s, int *target) {
	long lnum;
	int num;
	char* end;
	char* s1 = &s;

	errno = 0;
	lnum = strtol(s1, &end, 10);
	if(end==s1) {
		//fprintf(stderr, "ERROR: Can't convert string to number\n");
		return 0;
	}

	if((lnum == LONG_MAX || lnum == LONG_MIN) && errno == ERANGE) {
		// fprintf(stderr, "ERROR: number out of range for LONG\n");
		return 0;
	}
	
	if((lnum > INT_MAX) || (lnum < INT_MIN) ) {
		//fprintf(stderr, "ERROR: number out of range for INT\n");
		return 0;
	}

	num = (int) lnum;
	if(target) (*target) = num;
	return 1;
}

void string_print(string s) {
	printf("%.*s", (int) s.count, s.data);
}

string string_chop_left(string *s, size_t n) {
	if(n > s->count) {
		n = s->count;
	}

	string result = string_from_parts(s->data, n);

	s->data += n;
	s->count -= n;

	return result;
}

