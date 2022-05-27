#include "HttpServer.h"

void Server() {
    struct sockaddr_in server_addr;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server_addr, sizeof(server_addr)); //clear label
    server_addr.sin_family = AF_INET;//ipv4
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//listen ip addr
    server_addr.sin_port = htons(SERVER_PORT);//post
    bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    fprintf(stdout, "The Server waiting for  connection:\n");
    listen(sock, LISTEN_NUM);
    while (1) {
        // accept sock
        struct sockaddr_in client;
        int clientSock;
        socklen_t client_addr_len = sizeof(client);
        clientSock = accept(sock, (struct sockaddr *) &client, &client_addr_len);
        // create thread
        pthread_t id;
        int *pClientSock = NULL;
        pClientSock = (int *) malloc(sizeof(int));
        *pClientSock = clientSock;
        pthread_create(&id, NULL, doHttpRequest, (void *) pClientSock);
    }
}

/**
 *  function: parse request head and
 *  param:int sock
 *  return:void
 *  Implementation steps：
 *      1. read first line
 *      2. parse out the Method  and url
 *      3. call function:GETRequest get method
 *                       POSTRequest  post method
 *                       501 error
 * */
void *doHttpRequest(void *pClientSock) {
    //1. read first line
    int clientSock = *(int *) pClientSock;
    int len = 0;
    char buf[BUF_LEN], method[10], url[256];
    len = getLine(clientSock, buf, sizeof(buf));
    if (len <= 0) {//read error
        fprintf(stderr, "request error\n");
        if (pClientSock)
            free(pClientSock);
        close(clientSock);
        return NULL;
    }


    //2. param out method and url
    int i = 0, j = 0;
    while (!isspace(buf[j]) && i < sizeof(method) - 1)
        method[i++] = buf[j++];
    method[i] = '\0';
    //read url
    while (isspace(buf[j++]));//remove  space
    i = 0;
    while (!isspace(buf[j]) && i < sizeof(url) - 1)
        url[i++] = buf[j++];
    url[i] = '\0';

    //3. Call the corresponding method
    if (strncasecmp(method, "GET", i) == 0)
        GETRequest(clientSock, url);
    else if (strncasecmp(method, "POST", i) == 0)
        POSTRequest(clientSock, url);
    else
        NotImplemented(clientSock);

    if (pClientSock)
        free(pClientSock);
    close(clientSock);
    return NULL;
}
/**
 * function: deal get request
 * param: int sock; char *url
 * return:void
 * Implementation steps:1.read all request head and print
 *                      2.parse out real url remove ? after the data
 *                      3.check that the data exist
 *                      4.set request header and send
 *                          judge request type
 *                      5.response body info
 *
 * */
void GETRequest(int clientSock, char *url) {

    // 1.read all request head and print
    printf("\n-------------------GET--request--Start---------------------\n");
    fprintf(stdout, "Method:GET\nURL:%s\n", url);
    int len = 0;
    char buff[BUF_LEN];
    do {
        len = getLine(clientSock, buff, sizeof(buff));
        if (len < 0) {
            fprintf(stderr, "Read Get Head Error. reason:%s\n", strerror(errno));
            close(clientSock);
            return;
        }
        printf( "%s\n", buff);
    } while (len > 0);
    printf("-------------------GET--request--End---------------------\n\n");

    // 2.parse out real url;remove ? after the data
    char *pos = strchr(url, '?');//get real url
    if (pos)
        *pos = '\0';

    // 3.check that the data exist;if not exist return
    struct stat stUrl;
    if (stat(url, &stUrl) == -1) {
        NotFound(clientSock,url);
        return;
    }

    // 4. set request header
    //printf("\n-------------------GET Response Start------------------------------------\n");
    char head_buff[] = "HTTP/1.1 200 OK\r\n"
                       "Server:HaiZiChe Server\r\n"
                       "Connection: close\r\n";
    char temp[64];//content_len
    snprintf(temp, 64, "Content-Length: %ld\r\n", stUrl.st_size);
    strcat(head_buff, temp);
    const char *type;
    int modes = 0;
    type = strrchr(url, '.');
    if (strcmp(type, ".html") == 0)
        strcat(head_buff, "Content-Type: text/html;charset=UTF-8\r\n\r\n");
    else if (strcmp(type, ".css") == 0)
        strcat(head_buff, "Content-Type: text/css;charset=UTF-8\r\n\r\n");
    else if (strcmp(type, ".js") == 0)
        strcat(head_buff, "Content-Type: text/javascript;charset=UTF-8\r\n\r\n");
    else if (strcmp(type, ".png") == 0) {
        strcat(head_buff, "Content-Type: image/png\r\n\r\n");
        modes = 1;
    } else if (strcmp(type, ".jpg") == 0) {
        strcat(head_buff, "Content-Type: image/jpg\r\n\r\n");
        modes = 1;
    } else if (strcmp(type, ".ico") == 0) {
        strcat(head_buff, "Content-Type: image/x-icon\r\n\r\n");
        modes = 1;
    } else if (strcmp(type, ".gif") == 0) {
        strcat(head_buff, "Content-Type: image/gif\r\n\r\n");
        modes = 1;
    }

    // 5.send response header
    len = write(clientSock, head_buff, strlen(head_buff));
    if (len < 0) {
        fprintf(stderr, "sent head error. reason:%s\n", strerror(errno));
        close(clientSock);
        return;
    }

    // 6. send info body
    if (modes)//images type
        SendImage(url, clientSock);
    else {
        FILE *resource = NULL;
        resource = fopen(url, "r");
        char body_buff[1024];
        while (!feof(resource)) {
            fgets(body_buff, sizeof(buff), resource);
            len = write(clientSock, body_buff, strlen(body_buff));
            if (len < 0) {
                fprintf(stderr, "Send body error reason:%s\n", strerror(errno));
                break;
            }
        }
        fclose(resource);
    }
    //printf("\n-------------------GET Response End------------------------------------\n");
    printf("GET Response Success");
}
void POSTRequest(int clientSock, char *url) {
    printf("\n-------------------------POST Request Start-------------------------------\n");
    printf("Method:POST\nURL:%s",url);
    int len = 0;
    char buff[BUF_LEN];
    do {
        len = getLine(clientSock, buff, sizeof(buff));
        if (len < 0) {
            fprintf(stderr, "Read Post Header Error. reason:%s\n", strerror(errno));
            return;
        }
        printf("%s\n", buff);
    } while (len > 0);
    char data[512];
    len = read(clientSock, data, sizeof(data));
    printf("Data:%s\n", data);
    printf("\n-------------------------POST Request End-------------------------------\n");

    /*
     * If your POST method needs to check the request parameters
     * The following code may be useful to you
     * If not,you can remove some code
     * */

    //set statues ,default is 1;
    // 1:response 200 OK; 0:response 400 Bad Request
    int status = 1;

    //The result is used  return
    //You can call a function that you wrote yourself to return the result
    //You can write functions like this: char* Process(char *data,int *statues)
    //Note this, 195-line var data is a parameter received by POST
    //If request parameters are incorrect, you can set statues to 0
    char *result =  "";

    //set response header
    char header_200[1024]="HTTP/1.1 200 OK\r\n";
    char *header_400="HTTP/1.1 400 Bad Request\r\n";
    char *head_buff =
            "text/plain\r\n"
            "Server:HaiZiChe Server\r\n"
            "Connection: close\r\n";
    //if request parameters correct
    if(status){
        strcat(header_200,head_buff);
        snprintf(header_200,1024, "Content-Length: %ld\r\n\r\n", strlen(result));
        //send response header
        write(clientSock, header_200, sizeof(header_200));
        //send response body
        write(clientSock, result, sizeof(result));
    }
    else{//If parameters incorrect you can response 400
        strcat(header_400,head_buff);
        strcat(header_400,"Content-Length: 0\r\n\r\n");
        if(write(clientSock, header_400, sizeof(header_400))<0);
        fprintf(stderr,"Send 400 failed.  Reason:%s", strerror(errno));

    }
    close(clientSock);





}
/**
 * function:The request method is neither get nor post return 501 and close
 * param:int sock
 * return:void
 * */
void NotImplemented(int clientSock) {//501
    fprintf(stderr, "Method Not Implemented\n");
    int len = 0;
    char buf[BUF_LEN];
    do {
        if (getLine(clientSock, buf, sizeof(buf)) < 0)
            break;
    } while (len > 0);
    char head_buff[] = "HTTP/1.1 501 Not Implemented\r\n"
                       "Server:Haiziche Server\r\n"
                       "Content-Type: text/html;charset=UTF-8\r\n"
                       "Connection: close\r\n"
                       "Content-Length:0\r\n\r\n";
    if(write(clientSock, head_buff, strlen(head_buff))<=0)
        fprintf(stderr,"Send head failed. reason:%s\n",strerror(errno));
    close(clientSock);
}
void NotFound(int clientSock,char  *url) {
    printf("\n404 Not Found.  URL:%s\n",url);
    struct stat st;
    char head_buf[] = "HTTP/1.1 404 Not Found\r\n"
                      "Server:HaiZiChe Server\r\n"
                      "Content-Type: text/html;charset=UTF-8\r\n"
                      "Connection: close\r\n";
    FILE *resource = NULL;
    resource = fopen("404.html", "r");
    if (!resource || stat("404.html", &st) == -1) {
        fprintf(stderr, "404.html not found\n");
        close(clientSock);
        return;
    }
    char temp[64];
    snprintf(temp, 64, "Content-Length: %ld\r\n\r\n", st.st_size);
    strcat(head_buf, temp);
    int len = write(clientSock, head_buf, strlen(head_buf));
    if (len <= 0)
        fprintf(stderr, "send reply failed. reason:%s\n", strerror(errno));
    char body_buf[1024];
    while (!feof(resource)) {
        fgets(body_buf, sizeof(body_buf), resource);
        len = write(clientSock, body_buf, strlen(body_buf));
        if (len < 0) {
            fprintf(stderr, "send body error reason:%s\n", strerror(errno));
            break;
        }
    }
    fclose(resource);
    close(clientSock);

}

/**
 * function:send image to client
 * param: url sock
 * return:void
 * implementation steps: open read info; write back info
 * */
void SendImage(char *url, int clientSock) {
    ifstream img(url, std::ifstream::in);
    img.seekg(0, img.end);
    int length = img.tellg();
    img.seekg(0, img.beg);
    char *buffer = new char[length];
    img.read(buffer, length);
    int ret = write(clientSock, buffer, length);
    if (ret < 0)
        fprintf(stderr, "send body error reason:%s", strerror(errno));
}


/** function:read a line
 *  param:  sock
 *          buf:stores the data read
 *          size: the data length
 *  return:  0 :empty line
 *          -1: error
 *          Positive integer：read success
 * */
int getLine(int sock, char *buf, int size) {
    int cnt = 0;//count
    char ch = '\0';
    int len = 0;
    while (cnt < size - 1) {
        len = read(sock, &ch, 1);
        if (len == 1) {
            if (ch == '\r')
                continue;
            else if (ch == '\n')
                break;
            buf[cnt] = ch;
            cnt++;
        } else if (len == -1) {
            perror("read failed\n");
            cnt = -1;
            break;
        } else {//client close connect
            fprintf(stderr, "client close\n");
            cnt = -1;
            break;
        }
    }
    if (cnt >= 0)
        buf[cnt] = '\0';
    return cnt;
}
