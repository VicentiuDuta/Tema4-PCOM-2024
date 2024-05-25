#include <iostream>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "json.hpp"
#include "helpers.h"
#include "requests.h"

#define PORT 8080
#define MAXLEN 1024

const char* IP = "34.246.184.49";

using namespace std;
using json = nlohmann::json;

string get_cookie(char* response) {
    char* cookie = strstr(response, "Set-Cookie: ");
    if(cookie == NULL) {
        return "";
    }
    cookie += 12;
    string cookie_str = "";
    while(*cookie != ';') {
        cookie_str += *cookie;
        cookie++;
    }
    return cookie_str;
}

string get_token(char* response) {
   char *token = strstr(response, "token");
   char delim[] = ":\"";

   if (token != nullptr) {
       strtok(token, delim);
       return string(strtok(nullptr, delim));
   }

   return "";
}

bool is_number(const string& s) {
    return s.find_first_not_of("0123456789") == string::npos;
}


int main() {
    string cookie; 
    string token;

    while(1) {
        string command;
        cin >> command;

        if(command == "register") {
            cin.ignore();
            if(cookie != "") {
                cout << "[EROARE]: Utilizatorul este deja logat" << endl;
                continue;
            }

            string username, password;
            printf("username=");
            getline(cin, username);
            printf("password=");
            getline(cin, password);

            // daca username-ul sau parola contin spatii, afisez eroare
            if(username.find(' ') != string::npos || password.find(' ') != string::npos) {
                cout << "[EROARE]: Username-ul si parola nu trebuie sa contina spatii" << endl;
                continue;
            }

            //fac mesaj json
            json j;
            j["username"] = username;
            j["password"] = password;

            //fac mesajul de post
            const char* path = "/api/v1/tema/auth/register";
            const char* content_type = "application/json";

            // Creez mesajul de post
            char* message = compute_post_request(IP, path, content_type, NULL, j);

            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            if(code[0] == '2') {
                cout << "Utilizator inregistrat cu SUCCES" << endl;
            } else {
                cout << "EROARE la inregistrare" << endl;
            }

            // Inchid conexiunea
            close_connection(sockfd);
            continue;
        }

        else if(command == "login") {
            cin.ignore();
            string username, password;
            printf("username=");
            getline(cin, username);

            printf("password=");
            getline(cin, password);

            if(username.find(' ') != string::npos) {
                cout << "[EROARE]: Username-ul nu trebuie sa contina spatii" << endl;
                continue;
            }
            if(password.find(' ') != string::npos) {
                cout << "[EROARE]: Parola nu trebuie sa contina spatii" << endl;
                continue;
            }

            if(cookie != "") {
                cout << "[EROARE]: Utilizatorul este deja logat" << endl;
                continue;
            }

            //fac mesaj json
            json j;
            j["username"] = username;
            j["password"] = password;

            const char* path = "/api/v1/tema/auth/login";
            const char* content_type = "application/json";

            // Creez mesajul de post
            char* message = compute_post_request(IP, path, content_type, NULL, j);

            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            if(code[0] == '2') {
                cout << "Utilizator logat cu SUCCES" << endl;
            } else {
                cout << "EROARE la logare" << endl;
            }

            // Inchid conexiunea
            close_connection(sockfd);

            // Extrag cookie
            cookie = get_cookie(response);
            continue;
        }

        else if(command == "enter_library") {
            if(cookie == "") {
                cout << "[EROARE]: Utilizatorul nu este logat" << endl;
                continue;
            }
            // fac mesajul de get
            const char* path = "/api/v1/tema/library/access";
            char* message = compute_get_request(IP, path, (char*)cookie.c_str(), NULL);
            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            if(code[0] == '2') {
                cout << "[SUCCES]: Utilizatorul are acces la biblioteca" << endl;
                // Extrag token
                token = get_token(response);
                continue;
            } else {
                cout << "EROARE la accesarea bibliotecii" << endl;
                continue;
            }

            // Inchid conexiunea
            close_connection(sockfd);

            continue;
        }

        else if(command == "get_books") {
            if(cookie == "") {
                cout << "[EROARE]: Utilizatorul nu este logat" << endl;
                continue;
            }

            if(token == "") {
                cout << "[EROARE]: Utilizatorul nu are acces la biblioteca" << endl;
                continue;
            }
            // fac mesajul de get
            const char* path = "/api/v1/tema/library/books";
            char* message = compute_get_request(IP, path, (char*)cookie.c_str(), (char*)token.c_str());

            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
            
            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            // Afisez raspunsul
            if(code[0] == '2') {
                cout << "Cartile au fost primite cu SUCCES" << endl;
            } else {
                cout << "EROARE la primirea cartilor" << endl;
                continue;
            }
            // Inchid conexiunea
            close_connection(sockfd);

            // extrag cartile
            char* books = basic_extract_json_response(response);
            if(books != NULL)
                cout << books << endl;

            continue;
        }

        else if(command == "get_book") {
            if(cookie == "") {
                cout << "[EROARE]: Utilizatorul nu este logat" << endl;
                continue;
            }

            if(token == "") {
                cout << "[EROARE]: Utilizatorul nu are acces la biblioteca" << endl;
                continue;
            }

            string id;
            cout << "id=";
            cin >> id;

            if(id.find_first_not_of("0123456789") != string::npos) {
                cout << "[EROARE]: Id-ul trebuie sa fie un numar" << endl;
                continue;
            }
            
            // fac mesajul de get
            string path = "/api/v1/tema/library/books/";
            path += id;

            char* message = compute_get_request(IP, (char*)path.c_str(), (char*)cookie.c_str(), (char*)token.c_str());
            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;

            if(code[0] == '2') {
                cout << "Cartea a fost primita cu SUCCES" << endl;
            } else {
                cout << "EROARE la primirea cartii" << endl;
                continue;
            }

            // Inchid conexiunea
            close_connection(sockfd);

            // extrag cartea
            char* book = basic_extract_json_response(response);
            if(book != NULL)
                cout << book << endl;
            else
                cout << "Cartea cu id: " << id << " nu exista" << endl; 

            continue;
        }

        else if(command == "add_book") {
            cin.ignore();
            if(cookie == "") {
                cout << "[EROARE]: Utilizatorul nu este logat" << endl;
                continue;
            }

            if(token == "") {
                cout << "[EROARE]: Utilizatorul nu are acces la biblioteca" << endl;
                continue;
            }
            string title, author, genre, publisher, page_count;
            cout << "title=";
            getline(cin, title);

            cout << "author=";
            getline(cin, author);

            cout << "genre=";
            getline(cin, genre);

            cout << "publisher=";
            getline(cin, publisher);

            cout << "page_count=";
            getline(cin, page_count);

            // verific daca page_count este numar
            if(!is_number(page_count)) {
                cout << "[EROARE]: page_count trebuie sa fie un numar" << endl;
                continue;
            }

            //verific daca author, title, genre, publisher, page_count nu sunt goale
            if(author == "" || title == "" || genre == "" || publisher == "") {
                cout << "[EROARE]: author, title, genre, publisher, page_count nu trebuie sa fie goale" << endl;
                continue;
            }

            // fac mesaj json
            json j;
            j["title"] = title;
            j["author"] = author;
            j["genre"] = genre;
            j["page_count"] = page_count;
            j["publisher"] = publisher;


            // fac mesajul de post
            const char* path = "/api/v1/tema/library/books";
            const char* content_type = "application/json";

            // Creez mesajul de post
            char* message = compute_post_request(IP, path, content_type, (char*)token.c_str(), j);

            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            if(code[0] == '2') {
                cout << "Carte adaugata cu SUCCES" << endl;
            } else {
                cout << "EROARE la adaugarea cartii" << endl;
                close_connection(sockfd);
                continue;
            }


            // Inchid conexiunea
            close_connection(sockfd);

            continue;

        }

        else if(command == "delete_book") {
            cout << "id=";
            int id;
            cin >> id;

            // fac mesajul de delete
            string path = "/api/v1/tema/library/books/";
            path += to_string(id);

            char* message = compute_delete_request(IP, (char*)path.c_str(), (char*)token.c_str());

            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            if(code[0] == '2') {
                cout << "Cartea cu id " << id << " a fost stearsa cu SUCCES" << endl;
            } else {
                cout << "EROARE la stergerea cartii" << endl;
                close_connection(sockfd);
                continue;
            }


            // Inchid conexiunea
            close_connection(sockfd);

            continue;
        }

        else if(command == "logout") {
            if(cookie == " ") {
                cout << "[EROARE]: Utilizatorul nu este logat" << endl;
                continue;
            }

            // fac mesajul de get
            const char* path = "/api/v1/tema/auth/logout";
            char* message = compute_get_request(IP, path, (char*)cookie.c_str(), (char*)token.c_str());

            // Creez conexiunea
            int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            // Trimit mesajul
            send_to_server(sockfd, message);

            // Primesc raspunsul
            char* response = receive_from_server(sockfd);

            // extrag codul de raspuns din mesaj
            char* code = response + 9;
            if(code[0] == '2') {
                cout << "[SUCCESS]: Utilizatorul a fost delogat cu SUCCES" << endl;
            } else {
                cout << "EROARE la delogare" << endl;
                close_connection(sockfd);
                continue;
            }

            // Inchid conexiunea
            close_connection(sockfd);

            cookie = "";
            token = "";

            continue;
        }

        else if(command == "exit") {
            break;
        }

        else {
            cout << "[EROARE]: Comanda invalida" << endl;
            continue;
        }
    }

    return 0;
}