#include <stdio.h>
#include "lexc_err.h"


void lexc_error(int err_code) {
  	if(err_code)
    	printf("\n\tError #%d: %s\n\n", err_code, err_msg[err_code]);
}
