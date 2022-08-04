#include "./buffer_color.h"

void text_read_file(Text *text, const char* file_path) {
  size_t size;

  char *content = slurp_file(file_path, &size);
  printf("%s\n", file_path);

  string s = string_from_parts(content, size);
  string s_backup = string_from_parts(content, size);
  int n;
  size_t max_size=0;
  while(s.count) {
    string line = string_chop_by_delim(&s, '\n');

    n = (int) line.data[0] - '0';
    if(n<0 || n>3) {
      fprintf(stderr, "ERROR: Can not parse '%s': %c is not within the range 0-3\n",
	      file_path, line.data[0]);
      exit(1);       
    }

    bool is_word = line.data[2]==':';
    string word = string_chop_by_delim(&line, ' ');
    if(is_word) {
      while(line.count) {
	string_chop_by_delim(&line, ' ');
	max_size = max_size < word.count ? word.count : max_size;
	text->keywords_count++;
      }
    }
    else {
      text->sentence_count++;
    }
  }
  max_size++;

  //ALLOC

  //KEYWORDS
  text->keywords = (char **) malloc(text->keywords_count * sizeof(char *));
  if(text->keywords==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->keywords\n"),
      exit(1);
  }
  text->keywords_len = (size_t *) malloc(text->keywords_count * sizeof(size_t));
  if(text->keywords_len==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->keywords_len\n"),
      exit(1);
  }
  text->keywords_colors = (int *) malloc(text->keywords_count * sizeof(int));
  if(text->keywords_colors==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->keywords_colors\n"),
      exit(1);
  }
  
  for(size_t i=0;i<text->keywords_count;i++) {
    text->keywords[i] = (char *) malloc(max_size * sizeof(char));
    if(text->keywords[i]==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->keywords\n"),
      exit(1);
    }
  }

  //SENTENCES
  text->sentence_open = (char *) malloc(text->sentence_count * sizeof(char));
  if(text->sentence_open==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->sentence_open\n"),
      exit(1);
  }
  text->sentence_closed = (char *) malloc(text->sentence_count * sizeof(char));
  if(text->sentence_closed==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->sentence_closed\n"),
      exit(1);
  }
  text->sentence_colors = (int *) malloc(text->sentence_count * sizeof(int));
  if(text->sentence_colors==NULL) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: text->sentence_colors\n"),
      exit(1);
  }

  int ki=0;
  int i=0;
  while(s_backup.count) {
    string line = string_chop_by_delim(&s_backup, '\n');

    n = (int) line.data[0] - '0';
    bool is_word = line.data[2]==':';
    
    string word = string_chop_by_delim(&line, ' ');
    if(is_word) {
      while(line.count) {
	word = string_trim(string_chop_by_delim(&line, ' '));
	memcpy(text->keywords[ki], word.data, word.count);
	text->keywords[ki][word.count] = 0;
	text->keywords_len[ki] = word.count;
	text->keywords_colors[ki++] = n;
      }
    }
    else {
      word = string_trim(string_chop_by_delim(&line, ' '));
      text->sentence_open[i] = word.data[0];
      text->sentence_closed[i] = word.data[1];
      text->sentence_colors[i++] = n;      
    }
  }

  free(content);
}

void text_dump(const Text *text) {
  printf("keywords_count: %lld\n", text->keywords_count);
  for(size_t i=0;i<text->keywords_count;i++) {
    printf("\t%s(%lld): %d\n",
	   text->keywords[i], text->keywords_len[i], text->keywords_colors[i]);
  }
  printf("\n\n");
  printf("sentence_count: %lld\n", text ->sentence_count);
  for(size_t i=0;i<text->sentence_count;i++) {
    printf("\t%c - %c: %d\n",
	   text->sentence_open[i],
	   text->sentence_closed[i],
	   text->sentence_colors[i]
	   );
  }
}

void text_check_ptr(const Text *text) {
  if(!text->lines) {
    fprintf(stderr, "ERROR: Can not allocate enough memory for lines\n");
    exit(1);
  } 
}

void text_check_lines_ptr(const Text *text, size_t start) {
  for(size_t i=start;i<text->lines_cap;i++) {
    Line *line = &text->lines[i];
    if(!line->intervals) {
      fprintf(stderr, "ERROR: Can not allocate enough memory for lines\n");
      exit(1);      
    }    
  }
}

void text_init(Text *text) {
  text->lines = (Line *) malloc(sizeof(Line) * LINES_SIZE);
  text->lines_cap = LINES_SIZE;
  for(size_t i=0;i<text->lines_cap;i++) {
    Line *line = &text->lines[i];
    line->intervals = (Interval *) malloc(sizeof(Interval) * INTERVALS_SIZE);
    line->intervals_cap = INTERVALS_SIZE;
  }

  text_check_ptr(text);
  text_check_lines_ptr(text, 0);
}

void text_quit(Text *text) {
  for(size_t i=0;i<text->lines_cap;i++) {
    free(text->lines[i].intervals);
  }
  free(text->lines);

  for(size_t i=0;i<text->keywords_count;i++) {
    free(text->keywords[i]);
  }  
  free(text->keywords);
  free(text->keywords_len);
  free(text->keywords_colors);
  
  //SENTENCES
  free(text->sentence_open);
  free(text->sentence_closed);
  free(text->sentence_colors);
}

void text_line_alloc(Text *text) {

  text->lines = (Line *) realloc(text->lines, sizeof(Line) * text->lines_cap*2);
  
  text_check_ptr(text);
  
  for(size_t i=0;i<text->lines_cap;i++) {
    Line *line = &text->lines[i+text->lines_cap];
    line->intervals = (Interval *) malloc(sizeof(Interval) * INTERVALS_SIZE);
    line->intervals_cap = INTERVALS_SIZE;
  }

  size_t pos = text->lines_cap;
  text->lines_cap *= 2;
  text_check_lines_ptr(text, pos);
}

void text_intervall_alloc(Text *text, size_t index) {

  text->lines[index].intervals_cap *= 2;
  text->lines[index].intervals = (Interval *) realloc(text->lines[index].intervals,
						      sizeof(Interval) * text->lines[index].intervals_cap);
  
  text_check_lines_ptr(text, 0);
}

/*
#define KEYWORDS_CAP 12
static char *keywords[KEYWORDS_CAP] = {
  "if", "while", "for", "const", "static", "typedef", "struct", "#include", "#define", "return"};
static size_t keywords_len[] = { 2, 5, 3, 5, 6, 7, 6, 8, 7, 6};
static int keywords_colors[] = { 1, 1, 1, 1, 1, 1, 1, 2, 2, 1};
static size_t keywords_count = 10;//

static char sentence_open[] = {'\"', '<'};
static char sentence_close[] = {'\"', '>'};
static int sentence_colors[] = {3, 3};
static size_t sentence_count = 2;
*/

bool line_eol(const Text *text, char *buffer, size_t len, int *color, size_t *end) {
  for(size_t i=0;i<len;i++) {
    for(size_t q=0;q<text->sentence_count;q++) {      
      if(buffer[i]==text->sentence_open[q] && (i==0 || buffer[i-1]!='\\')) {
	bool open = true;
	size_t p; 
	for(p=i+1;p<len;p++) {
	  if(buffer[p]==text->sentence_closed[q] && buffer[p-1]!='\\') {
	    open = false;
	    p++;
	    break;
	  }
	}
	if(!open) {
	  if(i==0) {
	    if(end) *end=p;
	    if(color) *color=text->sentence_colors[q];
	    return p-1==len-1;
	  }
	  else {
	    if(end) *end=i;
	    if(color) *color=0;
	    return i==len-1;
	  }
	}
      }
    }
    for(size_t k=0;k<text->keywords_count;k++) {
      bool candidate = true;
      if(i+text->keywords_len[k]>len) continue;
      for(size_t j=0;j<text->keywords_len[k];j++) {
	if(buffer[i+j]!=text->keywords[k][j]) {
	  candidate = false;
	  break;
	}
      }
      if(candidate) {
	if(i==0) {
	  if(end) *end=text->keywords_len[k];
	  if(color) *color=text->keywords_colors[k];
	  return text->keywords_len[k]==len;
	}
	else {
	  if(end) *end=i;
	  if(color) *color=0;
	  return i==len-1;
	}      
      }
    }
  }

  if(end) *end=len;
  if(color) *color=0;
  return true;
}

bool isText(char *d) {
  char c = d[0];
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

void text_update(Text *text, const Buffer *buffer, const Cursor *cursor, size_t y, size_t height) {
  (void) cursor;
  text->lines_count = 0;
  for(size_t i=y;i<(y+height) && i<buffer->lines_size;i++) {
    if(text->lines_count>=text->lines_cap) {
      text_line_alloc(text);
    }
    
    Line *line = &text->lines[text->lines_count];
    line->line = i;
    line->intervals_count = 0;
        
    size_t end=0;
    size_t acc=0;
    int special = 0;
    bool eol;
    while(true) {
      eol = line_eol(text, buffer->buffer+buffer->positions[i] + acc, buffer->lines[i] - acc, &special, &end);

      if(special) {
	if(acc>0 && isText(buffer->buffer + buffer->positions[i] + (acc-1))) {
	  special = 0;
	}
	if((acc+end-1<buffer->lines[i]) && isText(buffer->buffer + buffer->positions[i] + (acc+end))) {
	  special = 0;
	}
      }
      //ADD INTERVAL
      if(line->intervals_count >= line->intervals_cap) {
	text_intervall_alloc(text, text->lines_count);
      }
      Interval *inter = &line->intervals[line->intervals_count];	   
      inter->start = acc;
      inter->end = end;
      inter->color = special;
      
      line->intervals_count++;
      acc+=end;
          
      if(eol) {
	break;
      }
    }

    //ADD LINE
    text->lines_count++;
  }
}

