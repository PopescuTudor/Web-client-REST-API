#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <iostream>

#include "helpers.hpp"
#include "requests.hpp"
// am folosit nlohmann pentru a parsa json
#include "json.hpp"

#define BUFFLEN 100
#define CLEN 300
#define JWTLEN 500
#define MESSAGELEN 1000

using namespace std;
// pentru nlohmann
using json = nlohmann::json;
bool check_format_add_book(char* field, char *buffer) {
    if (strcmp(buffer,"") == 0) {
        return false;
    }
    else if (strcmp(field, "page_count") == 0) { //verific ca page_count sa fie numar
        for(int i = 0; i < std::strlen(buffer); i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                return false;
            }
        }
    }
    return true;;
}
int main(int argc, char *argv[]) {
	// dezactivez buffering-ul
	setvbuf(stdout, NULL, _IONBF, BUFFLEN);
	// realizarea conexiunii + file descriptor
	char *host = (char *)malloc(15 * sizeof(char));
	std::strcpy(host, "34.254.242.81");
	int sockfd = open_connection(host, 8080,
		AF_INET, SOCK_STREAM, 0);
	// buffer-ul in care citim comenzile
	char buffer[BUFFLEN - 1], *msg;
	// auxiliaries;
	char *r;
	char **transmit = (char **) malloc(1 * sizeof(char*));
	char session_cookie[CLEN], aux[CLEN], jwt[JWTLEN];
	int loggedIn = 0, enteredLibrary = 0;
	// mai jos este implementata functionalitatea clientului
	while(1) {
		fgets(buffer, BUFFLEN, stdin);
		if (strcmp(buffer, "exit\n") == 0) {
			break;
		} 
        else if (strcmp(buffer, "logout\n") == 0) {
			if (loggedIn == 0) {
				cout << "ERROR: You have to be logged in." << endl;
				continue;
			}

			char route[30];
			std::strcpy(route, "/api/v1/tema/auth/logout");

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			// sending with the cookie
			char *cookie[1];
			cookie[0] = (char *) malloc(CLEN * sizeof(char));
			std::strcpy(cookie[0], session_cookie);

			msg = compute_get_request(host, route, NULL, cookie, 1);
    		send_to_server(sockfd, msg);
    		r = receive_from_server(sockfd);

    		// aici fac inapoi pe 0 cele doua variabile
    		// de care ma folosesc pentru a verifica
    		// daca sunt logat sau au acces la biblioteca
    		if (strstr(r, "OK\r\n")) {
    			loggedIn = 0;
    			enteredLibrary = 0;
    			memset(session_cookie, 0, CLEN);
    			memset(jwt, 0, JWTLEN);
    			cout << "Succesfully logged out" << endl;
    		} 
            else {
    			cout << "ERROR: Couldn't log out" << endl;
    		}
		} 
        else if (strcmp(buffer, "delete_book\n") == 0) {
			if (loggedIn == 0) {
				cout << "ERROR: You have to be logged in." << endl;
				continue;
			}
			if (enteredLibrary == 0) {
				cout << "ERROR: You have to enter the library first." << endl;
				continue;
			}

			char id[20];
			cout << "id=";
			fgets(id, 20, stdin);
			id[strlen(id) - 1] = '\0';

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			char route[30];
			std::strcpy(route, "/api/v1/tema/library/books/");
			std::strcat(route, id);

			char* msg = (char*) malloc(MESSAGELEN * sizeof(char));
			std::strcat(msg, "DELETE ");
			std::strcat(msg, route);
			std::strcat(msg, " HTTP/1.1\r\n");
			std::strcat(msg, "Authorization: Bearer ");
			std::strcat(msg, jwt);
			std::strcat(msg, "\r\nHost: ");
			std::strcat(msg, host);
			std::strcat(msg, "\r\n");
			std::strcat(msg, "\r\n");

			send_to_server(sockfd, msg);
    		r = receive_from_server(sockfd);
    		if (strstr(r, "OK\r\n")) {
    			cout << "Succesfully deleted book" << endl;
    		} 
            else {
    			cout << "Couldn't delete book (wrong id)" << endl;
    		}
		} 
        else if (strcmp(buffer, "add_book\n") == 0) {
			if (loggedIn == 0) {
				cout << "ERROR: You have to be logged in." << endl;
				continue;
			}
			if (enteredLibrary == 0) {
				cout << "ERROR: You have to enter the library first." << endl;
				continue;
			}

			char title[50], author[50], genre[20];
			char publisher[50], page_count[10];
			cout << "title=";
			fgets(title, 20, stdin);
			title[strlen(title) - 1] = '\0';
            if(check_format_add_book("title", title) == false) {
                cout << "ERROR: Wrong format for title" << endl;
                continue;
            }

			cout << "author=";
			fgets(author, 20, stdin);
			author[strlen(author) - 1] = '\0';
			if(check_format_add_book("author", author) == false) {
                cout << "ERROR: Wrong format for author" << endl;
                continue;
            }
            
            cout << "genre=";
			fgets(genre, 20, stdin);
			genre[strlen(genre) - 1] = '\0';
            if(check_format_add_book("genre", genre) == false) {
                cout << "ERROR: Wrong format for genre" << endl;
                continue;
            }
            
            cout << "publisher=";
			fgets(publisher, 20, stdin);
			publisher[strlen(publisher) - 1] = '\0';
            if(check_format_add_book("publisher", publisher) == false) {
                cout << "ERROR: Wrong format for publisher" << endl;
                continue;
            }

			cout << "page_count=";
			fgets(page_count, 20, stdin);
			page_count[strlen(page_count) - 1] = '\0';
			if(check_format_add_book("page_count", page_count) == false) {
                cout << "ERROR: Wrong format for page_count" << endl;
                continue;
            }

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			// sending with the cookie
			char *cookie[1];
			cookie[0] = (char *) malloc(CLEN * sizeof(char));
			std::strcpy(cookie[0], session_cookie);

			json jmsg;
			jmsg["title"] = title;
			jmsg["author"] = author;
			jmsg["genre"] = genre;
			jmsg["page_count"] = atoi(page_count);
			jmsg["publisher"] = publisher;
			string message = jmsg.dump();

			transmit[0] = (char*) malloc(MESSAGELEN * sizeof(char));
			std::strcpy(transmit[0], message.c_str());

			char route[30], payload[30];
			std::strcpy(route, "/api/v1/tema/library/books");
			std::strcpy(payload, "application/json");

			msg = compute_post_request(host, route, payload,
				transmit, 1, NULL, 0);
			
			// adaugarea header-ului Authorization
			char *p = strstr(msg, "Host");
			
			char *message_aux = (char*) malloc(MESSAGELEN * sizeof(char));
			strncpy(message_aux, msg, strlen(msg) - strlen(p));
			std::strcat(message_aux, "Authorization: Bearer ");
			std::strcat(message_aux, jwt);
			std::strcat(message_aux, "\r\n");
			std::strcat(message_aux, p);

    		send_to_server(sockfd, message_aux);
    		r = receive_from_server(sockfd);

    		if (strstr(r, "OK\r\n")) {
    			cout << "Succesfully added book." << endl;
    		} 
            else {
    			cout << "ERROR: Couldn't add the book" << endl;
    		}
		} 
        else if (strcmp(buffer, "get_book\n") == 0) {
			if (loggedIn == 0) {
				cout << "ERROR: You have to be logged in." << endl;
				continue;
			}
			if (enteredLibrary == 0) {
				cout << "ERROR: You have to enter the library first." << endl;
				continue;
			}

			char id[20];
			cout << "id=";
			fgets(id, 20, stdin);
			id[strlen(id) - 1] = '\0';

			char route[30];
			std::strcpy(route, "/api/v1/tema/library/books/");
			std::strcat(route, id);

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);
			
			// sending with the cookie
			char *cookie[1];
			cookie[0] = (char *) malloc(CLEN * sizeof(char));
			std::strcpy(cookie[0], session_cookie);

			msg = compute_get_request(host, route, NULL, cookie, 1);
			
			// adaugarea header-ului Authorization
			char *p = strstr(msg, "Host");
			int i = 0;
			while (p[i] != '\n') {
				i++;
			}
			char *message = (char*) malloc(MESSAGELEN * sizeof(char));
			strncpy(message, msg, strlen(msg) - strlen(p));
			std::strcat(message, "Authorization: Bearer ");
			std::strcat(message, jwt);
			std::strcat(message, "\r\n");
			std::strcat(message, p);
			
    		send_to_server(sockfd, message);
    		r = receive_from_server(sockfd);
			
			
    		if (strstr(r, "OK\r\n")) {
				char* jsonStart = strchr(r, '{');
    			std::string aux_str;
				json msg = json::parse(jsonStart);
				
				cout << "id:" << msg["id"] << endl;
    			cout << "title:"<< msg["title"] << endl;
    			cout << "author" << msg["author"] << endl;
    			cout << "publisher:" << msg["publisher"] << endl;
    			cout << "genre:" << msg["genre"] << endl;
    			cout << "page_count:" << msg["page_count"] << endl;
				
    		} 
            else if (strstr(r, "No book was found")) {
    			cout << "ERROR: The book with the id " << id << " cannot be" <<
    			" found" << endl;
    		} 
            else {
    			cout << "ERROR: Error from the server";
    		}
			
		} 
        else if (strcmp(buffer, "get_books\n") == 0) {
			if (loggedIn == 0) {
				cout << "ERROR: You have to be logged in." << endl;
				continue;
			}
			if (enteredLibrary == 0) {
				cout << "ERROR: You have to enter the library first." << endl;
				continue;
			}
			char route[30];
			std::strcpy(route, "/api/v1/tema/library/books");

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			// sending with the cookie
			char *cookie[1];
			cookie[0] = (char *) malloc(CLEN * sizeof(char));
			std::strcpy(cookie[0], session_cookie);

			msg = compute_get_request(host, route, NULL, cookie, 1);
			
			// adaugarea header-ului Authorization
			char *p = strstr(msg, "Host");
			int i = 0;
			while (p[i] != '\n') {
				i++;
			}
			char *message = (char*) malloc(MESSAGELEN * sizeof(char));
			strncpy(message, msg, strlen(msg) - strlen(p));
			std::strcat(message, "Authorization: Bearer ");
			std::strcat(message, jwt);
			std::strcat(message, "\r\n");
			std::strcat(message, p);

    		send_to_server(sockfd, message);
    		r = receive_from_server(sockfd);
			
    		// Afisarea mesajului in format human readable
    		if (strstr(r, "OK\r\n")) {
    			std::string aux_str;
    			json jmsg = json::parse(strstr(r, "\r\n["));
    			unsigned long int j;
    			char title[50];
    			int id;
    			std::string s_aux;
    			// Aici parsez mesajul de tip json
    			// si afisez cartile
    			if (strstr(r, "\r\n[]\r\n")) {
    				cout << "There are no books." << endl;
    				continue;
    			}
    			if (jmsg.size() != 0) {
        			for (j = 0; j < jmsg.size(); j++) {
        				s_aux = jmsg[j]["title"];
        				std::strcpy(title, s_aux.c_str());
        				id = jmsg[j]["id"];
        				cout << j + 1 << ". " <<"id: " << id << "\n " <<
						"title:" << title << endl;
    					
    				}
    			} 
                else {
    				cout << "There are no books." << endl;
    			}
    		} 
            else {
    			cout << "ERROR: Error from the server.";
    		}
		} 
        else if (strcmp(buffer, "enter_library\n") == 0) {
			if (loggedIn == 0) {
				cout << "ERROR: No user logged in. Please log in " <<
				"before entering the library." << endl;
				continue;
			}
			char route[30];
			std::strcpy(route, "/api/v1/tema/library/access");

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			// trimit si cookie-ul
			char *cookie[1];
			cookie[0] = (char *) malloc(CLEN * sizeof(char));
			std::strcpy(cookie[0], session_cookie);

			msg = compute_get_request(host, route, NULL, cookie, 1);
    		send_to_server(sockfd, msg);
    		r = receive_from_server(sockfd);
    		// extrag mesajul json in loc sa
    		// parsez manual
    		if (strstr(r, "OK")) {
    			cout << "Succesfully entered the library. Enjoy!" << endl;
    			std::string aux_str;
    			json jmsg = json::parse(strstr(r, "{\""));
    			aux_str = jmsg["token"];
    			std::strcpy(jwt, aux_str.c_str());
    			enteredLibrary = 1;
    		} 
            else {
    			cout << "ERROR: Couldn't enter the library." << endl;
    		}
		} 
        else if (strcmp(buffer, "login\n") == 0) {
			if (loggedIn == 1) {
				cout << "ERROR: Already logged in with a user" << endl;
				continue;
			}
			char username[20], password[20];
			cout << "username=";
			fgets(username, 20, stdin);
			username[strlen(username) - 1] = '\0';
			cout << "password=";
			fgets(password, 20, stdin);
			password[strlen(password) - 1] = '\0';

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			json jmsg;
			jmsg["username"] = username;
			jmsg["password"] = password;
			string message = jmsg.dump();
			transmit[0] = (char *) malloc(BUFFLEN * sizeof(char));
			std::strcpy(transmit[0], message.c_str());

			char route[30], payload[30];
			std::strcpy(route, "/api/v1/tema/auth/login");
			std::strcpy(payload, "application/json");

			msg = compute_post_request(host, route, payload,
				transmit, 1, NULL, 0);
    		send_to_server(sockfd, msg);
    		r = receive_from_server(sockfd);
			
    		// parsez raspunsul de la server si
    		// preiau cookie-ul de sesiune
    		if (strstr(r, "Set-Cookie") == NULL) {
    			if (strstr(r, "No account with this username!"))
    				cout << "ERROR: No account with this username." << endl;
				else if (strstr(r, "Credentials are not good!"))
					cout << "ERROR: Wrong passwd." << endl;
    			continue;
    		} 
            else if (strstr(r, "OK\r\n")) {
    			cout << "Succesfully logged in." << endl;
    			std::strcpy(aux, (strstr(r, "Set-Cookie")));
    			loggedIn = 1;
    			int i = 0;
    			// parsez si fac rost de session cookie
    			while (aux[i + 12] != ';' || aux[i + 13] != ' ') {
    				session_cookie[i] = aux[i + 12];
    				i++;
    			}
    			session_cookie[i] = '\0';
    		}
		} 
        else if (strcmp(buffer, "register\n") == 0) {
			// comanda register - similara cu login
			// doar ca fara cookie
			if (loggedIn == 1) {
				cout << "ERROR: You have to be logged out in order to" <<
				" register another user." << endl;
				continue;
			}

			char username[20], password[20];
			cout << "username=";
			fgets(username, 20, stdin);
			username[strlen(username) - 1] = '\0';
			int check = 0;
			for(int c = 0; c < std::strlen(username); c++) {
				if(username[c] == ' ') {
					
					check = 1;
					break;
				}
			}
			if(check == 1) {
				cout<< "ERROR: Username must not contain spaces!" << endl;
				continue;
			}

			cout << "password=";
			fgets(password, 20, stdin);
			password[strlen(password) - 1] = '\0';
			check = 0;
			for(int c = 0; c < std::strlen(password); c++) {
				if(password[c] == ' ') {
					check = 1;
					break;
				}
			}
			if(check == 1) {
				cout<< "ERROR: Password must not contain spaces!" << endl;
				continue;
			}

			sockfd = open_connection(host, 8080,
				AF_INET, SOCK_STREAM, 0);

			// compun mesajul json
			json jmsg;
			jmsg["username"] = username;
			jmsg["password"] = password;
			string message = jmsg.dump();
			transmit[0] = (char *) malloc(BUFFLEN * sizeof(char));
			std::strcpy(transmit[0], message.c_str());

			// compun ruta si payload-ul si le trimit catre server
			char route[30], payload[30];
			std::strcpy(route, "/api/v1/tema/auth/register");
			std::strcpy(payload, "application/json");
			
			msg = compute_post_request(host, route, payload,
				transmit, 1, NULL, 0);
    		send_to_server(sockfd, msg);
    		r = receive_from_server(sockfd);
    		// afisarea unui mesaj "human readable"
    		if (strstr(r, "Created"))
    			cout << "Client " << username <<
    		" has been registered." << endl;
    		else if (strstr(r, "taken"))
    			cout << "Error: The username " << username <<
    			" is taken." << endl;
    		else
    			cout << "Server error.";
		}
	}
	return 0;
}