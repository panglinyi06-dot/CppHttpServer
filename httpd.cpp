#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SERVER_STRING "Server: CppHttpServer/0.1\r\n"

using namespace std;

/* 工具函数 */
void error_die(const string &msg) {
    perror(msg.c_str());
    exit(1);
}

/* 读取一行 */
int get_line(int sock, string &out) {
    char c = '\0';
    int n;
    out.clear();

    while (c != '\n') {
        n = recv(sock, &c, 1, 0);
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                if (n > 0 && c == '\n')
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            out += c;
        } else {
            c = '\n';
        }
    }
    return out.size();
}

/* 404 */
void not_found(int client) {
    string res =
        "HTTP/1.0 404 NOT FOUND\r\n"
        SERVER_STRING
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>";

    send(client, res.c_str(), res.size(), 0);
}

/* 静态文件 */
void serve_file(int client, const string &path) {
    FILE *fp = fopen(path.c_str(), "r");
    if (!fp) {
        not_found(client);
        return;
    }

    string header =
        "HTTP/1.0 200 OK\r\n"
        SERVER_STRING
        "Content-Type: text/html\r\n\r\n";

    send(client, header.c_str(), header.size(), 0);

    char buf[1024];
    while (fgets(buf, sizeof(buf), fp)) {
        send(client, buf, strlen(buf), 0);
    }

    fclose(fp);
}

/* CGI 执行 */
void execute_cgi(int client,
                 const string &path,
                 const string &method,
                 const string &query,
                 const string &body)
{
    int cgi_output[2];
    int cgi_input[2];

    if (pipe(cgi_output) < 0 ||
        pipe(cgi_input) < 0) {
        return;
    }

    string status = "HTTP/1.0 200 OK\r\n";

    send(client,
         status.c_str(),
         status.size(),
         0);

    pid_t pid = fork();

    if (pid == 0) {

        dup2(cgi_output[1], STDOUT_FILENO);
        dup2(cgi_input[0], STDIN_FILENO);

        close(cgi_output[0]);
        close(cgi_input[1]);

        string meth_env =
            "REQUEST_METHOD=" + method;

        putenv(const_cast<char*>(meth_env.c_str()));

        if (method == "GET") {

            string query_env =
                "QUERY_STRING=" + query;

            putenv(const_cast<char*>(query_env.c_str()));
        }
        else if (method == "POST") {

            string len_env =
                "CONTENT_LENGTH=" +
                to_string(body.size());

            putenv(const_cast<char*>(len_env.c_str()));
        }

        execl("/bin/bash",
	      "bash",
	      path.c_str(),
	      NULL);

        exit(1);
    }
    else {

        close(cgi_output[1]);
        close(cgi_input[0]);

        // 写POST数据给CGI
        if (method == "POST" && !body.empty()) {

            write(cgi_input[1],
                  body.c_str(),
                  body.size());
        }

        close(cgi_input[1]);

        char buf[1024];

        int n;

        while ((n = read(cgi_output[0],
                          buf,
                          sizeof(buf))) > 0) {

            send(client,
                 buf,
                 n,
                 0);
        }

        close(cgi_output[0]);

        waitpid(pid, nullptr, 0);
    }
}
/* 处理请求 */
void handle_client(int client)
{
    string line;

    get_line(client, line);

    stringstream ss(line);

    string method, url;

    ss >> method >> url;

    // 默认首页
    if (url == "/") {
        url = "/post.html";
    }

    int content_length = 0;

    // 读取HTTP头
    string header;

    while (get_line(client, header) > 0 &&
           header != "\n") {

        if (header.find("Content-Length:") != string::npos) {

            string len = header.substr(15);

            while (!len.empty() &&
                  (len[0] == ' ' || len[0] == '\t')) {

                len.erase(0, 1);
            }

            content_length = stoi(len);
        }
    }

    // ⭐ 读取POST body
// ⭐ 读取POST body
	string body;

	if (method == "POST" &&
	    content_length > 0) {

	    char buffer[4096];

	    int total = 0;

	    while (total < content_length) {

		int n = recv(client,
		             buffer + total,
		             content_length - total,
		             0);

		if (n <= 0)
		    break;

		total += n;
	    }

	    body.assign(buffer, total);
	}

	string path = "./httpdocs" + url;

	struct stat st;

	if (stat(path.c_str(), &st) == -1) {

	    not_found(client);

	} else {

	    // CGI
	    if (st.st_mode & S_IXUSR) {

		execute_cgi(client,
		            path,
		            method,
		            "",
		            body);

	    } else {

		serve_file(client, path);
	    }
	}

	close(client);
}
/* 启动服务器 */
int startup(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) error_die("socket");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0)
        error_die("bind");

    if (listen(sock, 5) < 0)
        error_die("listen");

    return sock;
}

/* 主函数 */
int main() {
    int port = 6379;
    int server = startup(port);

    cout << "HTTP Server running on port " << port << endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);

        int client = accept(server,
                            (sockaddr*)&client_addr,
                            &len);

        if (client == -1) continue;

        cout << "New connection: "
             << inet_ntoa(client_addr.sin_addr)
             << endl;

        thread(handle_client, client).detach();
    }

    close(server);
    return 0;
}
