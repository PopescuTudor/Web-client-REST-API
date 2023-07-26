# Web-client-REST-API
Proiect in limbajul C ce implementeaza un client Web ce interactioneaza cu un REST API pentru a accesa si modifica o biblioteca fictiva.


Un detaliu important este ca am ales utilitarul nlohmann/json, extras de pe
github.com/nlohmann/json si inclus in arhiva temei. A facilitat realizarea 
temei intr-un timp mai scurt, decat daca as fi interpretat raspunsurile si cererile 
ca un sir de caractere simplu. La output, pot afisa selectiv campurile din obiectul
JSON printr-o simpla indexare.

Un alt element nou din aceasta tema a fost token-ul JWT, utilizat pentru a asigura
autenticitatea informatiei trimise.
Pentru a include tokenul JWT, am ales să descompun șirul de caractere obținut din funcțiile
din requests.cpp. Header-ul de autentificare (Authorization) este plasat înaintea header-ului
Host în cadrul cererii. 

Acolo unde este cazul, pentru a nu fi nevoit sa reintroduc datele de logare la fiecare mesaj
catre server, concatenez șirul de caractere care reprezintă cererea cu cookie-ul care indică
dacă utilizatorul este autentificat.

Toate aceste lucruri fac ca experienta de interactiune cu server-ul sa fie una
naturala, desi se intampla intr-un mediu fara GUI.

In linii mari, clientul e realizat de un program cu un "flow" simplu. Intr-un while,
astept comenzi si redeschid conexiunea atunci cand primesc o comanda valida.
In functie de aceasta, formulez corect cereri de tip GET (pentru obtinere de info)
sau POST (pt a trimite si modifica date la server). Apoi, interpretez raspunsul si
afisez in format human-readable. La baza interpretarii si formularii mesajelor sta 
formatul JSON, sustinut de fisierul header json.hpp.

Comenzile date de client:

    register: Verific dacă sunt deja autentificat cu un utilizator, în caz contrar 
    nu pot înregistra un nou utilizator. Citesc username-ul și parola, deschid conexiunea, 
    construiesc obiectul JSON care le conține, apoi compun mesajul pentru cererea de tip POST 
    (folosind funcția compute_post_request). Trimit mesajul și interpretez răspunsul serverului.

    login: Verific dacă sunt deja autentificat, în caz contrar nu pot efectua autentificarea. 
    Citesc username-ul și parola, deschid conexiunea și construiesc un mesaj JSON care le conține, 
    pe care îl trimit apoi către server (tot printr-o cerere de tip POST). După ce primesc răspunsul de la server, 
    îl interpretez. Dacă primesc cookie-ul de sesiune, îl stochez într-un șir de caractere în programul meu. 
    Setez loggedIn pe 1.

    enter_library: Verific dacă sunt autentificat (dacă nu sunt, nu pot accesa biblioteca). 
    Deschid conexiunea, construiesc cererea de tip GET, o trimit către server, 
    apoi preiau răspunsul de la server și îl interpretez. Dacă nu există eroare, 
    preiau tokenul JWT primit ca JSON și îl stochez în același mod ca și cookie-ul de sesiune.

    get_books: Verific dacă sunt autentificat și am acces la bibliotecă. 
    Dacă am, deschid conexiunea, construiesc cererea de tip GET 
    și adaug înaintea header-ului Host header-ul de autorizare (Authorization) 
    conform specificațiilor din enunțul proiectului (cu prefixul "Bearer" în fața tokenului). 
    Trimit cererea la server și, dacă nu primesc eroare, preiau mesajul JSON, îl parsez 
    și încerc să îl afișez într-un format mai ușor de înțeles (o listă de cărți).

    get_book: Comanda get_book este similară cu cea anterioară, dar trebuie să citesc ID-ul cărții 
    și să îl adaug la ruta cererii. Înainte de aceasta, verific dacă sunt conectat și am acces la bibliotecă. 
    Construiesc cererea de tip GET cu ajutorul funcției compute_get_request, 
    iar în interiorul acesteia adaug header-ul cu tokenul JWT (similar cu comanda anterioară). 
    Dacă primesc un mesaj cu OK de la server, preiau mesajul, îl transform în format JSON 
    și afișez detaliile despre carte într-un mod prietenos.

    add_book: Verific dacă am acces la bibliotecă și sunt autentificat. 
    Citesc detaliile cărții si le verific corectitudinea, deschid conexiunea, construiesc un mesaj JSON cu detaliile citite 
    și îl trimit sub forma unei cereri de tip POST către server. 
    Adaug și header-ul de autorizare (Authorization) și interpretez răspunsul de la server.

    delete_book: Verific dacă sunt autentificat și am acces la bibliotecă. 
    Citesc ID-ul cărții, deschid conexiunea, construiesc ruta cu ID-ul specificat 
    și apoi construiesc manual mesajul pentru cererea de ștergere 
    (similar cu modul în care funcțiile compute_get_request, compute_post_request și compute_get_request lucrează, 
    doar că de data aceasta o fac manual în funcția main). 
    Primește mesajul de la server și, dacă este OK, interpretez răspunsul de la server.

    logout: Verific dacă sunt autentificat, altfel nu am de unde să mă deloghez. 
    Trimit cererea de tip GET către server, primesc răspunsul și, dacă este OK, resetez cookie-ul și tokenul JWT, 
    iar variabilele loggedIn și enteredLibrary sunt setate la 0.

    exit: La comanda exit se întrerupe execuția programului.
