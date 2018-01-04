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

#include <stdio.h>

#include "http.h"
#include "string_sw.h"

char *make_http_header(int http_status, int content_type, int content_length)
{
    const char *str_status = NULL;
    const char *str_type = NULL;
    char http_header[4096];

    switch (http_status)
    {
        case OK: 
            str_status = "200 OK";
            break;
        
        case E_NOT_ALLOWED: 
            str_status = "403 Forbidden";
            break;
        
        case E_NO_FILE:
            str_status = "404 Not Found";
            break;
        
        default:
            return NULL;
    }

    switch (content_type)
    {
        case TYPE_HTML:
                str_type = "text/html";
                break;

        case TYPE_JPG:
                str_type = "image/jpeg";
                break;

        case TYPE_PNG:
                str_type = "image/png";
                break;
                
        case TYPE_GIF:
                str_type = "image/gif";
                break;

        case TYPE_JS:
                str_type = "application/javascript";
                break;

        case TYPE_CSS:
                str_type = "text/css";
                break;

        case TYPE_ERROR:
                str_type = "text/plain";
                break;

        default:
            return NULL;
    }

    snprintf(http_header, sizeof(http_header),
             "HTTP/1.0 %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
             str_status, str_type, content_length);

    return strdup(http_header); 
}

char *make_report(int connections, long long sent_bytes,
                  int html, int jpg, int png, int gif,
                  int js, int css, int error)
{
    char report[4096];

    snprintf(report, sizeof report,
             "<html><body><h2>Status Report for webserver</h2><p>\n"
             "%d connections / %lld bytes sent</p>\n"
             "<p><table border=1 cellspacing=1><tr>\n"
             "<th>html</th> <th>jpg</th> <th>png</th>"
             "<th>gif</th> <th>js</th> <th>css</th>"
             "<th>error</th></tr>\n<tr>"
             "<td>%d</td> <td>%d</td> <td>%d</td> <td>%d</td>"
             "<td>%d</td> <td>%d</td> <td>%d</td>\n"
             "</tr></table></p></body></html>\n",
             connections, sent_bytes, html, jpg, png, gif, js, css, error);
    
    return strdup(report);
}
