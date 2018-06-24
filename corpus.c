#include "corpus.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* corpus_select_text(const char* filename, int* length)
{
  FILE* file = fopen(filename, "r");

  if(file == NULL)
  {
    return NULL;
  }

  int buffer_size = 4096;
  int buffer_length = 0;
  char* buffer = (char*)malloc(buffer_size);

  if(buffer == NULL)
  {
    fclose(file);
    return NULL;
  }

  while(!(feof(file) || ferror(file)))
  {
    if(buffer_length >= buffer_size)
    {
      char* bigger_buffer = (char*)realloc(buffer, 2 * buffer_size);

      if(bigger_buffer == NULL)
      {
        fclose(file);
        free(buffer);
        return NULL;
      }

      buffer = bigger_buffer;
      buffer_size *= 2;
    }

    buffer_length += fread(buffer + buffer_length, 1, buffer_size - buffer_length, file);
  }

  fclose(file);

  int n_texts = 0;

  for(int i = 0; i <= buffer_length; i ++)
  {
    if((i >= buffer_length || buffer[i] == '\n') && i > 0 && buffer[i - 1] != '\n')
    {
      n_texts ++;
    }
  }

  int selected_text = rand() % n_texts;
  int finish = 0;

  for(finish = 0;; finish ++)
  {
    if((finish >= buffer_length || buffer[finish] == '\n') && finish > 0 && buffer[finish - 1] != '\n')
    {
      if(-- selected_text == -1)
      {
        break;
      }
    }
  }

  int start;

  for(start = finish - 1; start > 0 && buffer[start - 1] != '\n'; start --);

  free(buffer);

  char* text = (char*)malloc(finish - start);

  if(text == NULL)
  {
    return NULL;
  }

  memcpy(text, buffer + start, finish - start);
  *length = finish - start;

  return text;  
}
