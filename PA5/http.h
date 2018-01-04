//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #5
// December 6, 2017
//
// Hayun Lee
// Embedded Software Laboratory
// Sungkyunkwan University
//
//-----------------------------------------------------------

#ifndef _HTTP_H_
#define _HTTP_H_

enum
{
    TYPE_HTML,
    TYPE_JPG,
    TYPE_PNG,
    TYPE_GIF,
    TYPE_JS,
    TYPE_CSS,
    TYPE_ERROR
};

#define OK (200)
#define E_NOT_ALLOWED (403)
#define E_NO_FILE (404)

char *make_http_header(int http_status, int content_type, int content_length);

char *make_report(int connections, long long sent_bytes,
                  int html, int jpg, int png, int gif,
                  int js, int css, int error);

#endif /* _HTTP_H */

