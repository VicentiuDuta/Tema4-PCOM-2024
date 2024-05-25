                                                                    Tema 4 - Protocoale de Comunicatie - Client Web. Comunicatie cu REST API.
                                                                                    - Duta Vicentiu-Alecsandru - 321CC -

    1. Infrastructura:
        - Am folosit scheletul de cod din Laboratorul 9 - Protocolul HTTP, modificand functiile conmpute_get_request() si compute_post_request(). De asemenea, am adaugat si functia compute_delete_request().
        - Am ales sa folosesc lohmann pentru parsarea JSON, pentru ca a fost usor de utilizat in tema.
        - In fisierul client.cpp se afla implementarea clientului.
        - In fisierele requests.cpp si requests.h se afla implementarile functiilor pentru requesturi.
        - In fisierele buffer.h, buffer.cpp, helpers.cpp, helpers.h se gasesc functiile pentru conectarea la server, trimiterea respectiv primirea pachetelor.

    2. Implementare:
        - Cookie-ul de sesiune si token-ul sunt actualizate pe parcursul programului.
        - Incep o bucla infinita in care citesc comenzile:
            -> register : Citesc credentialele. Verific daca contin spatii, iar in cazul in care sunt valide, creez un JSON care contine username-ul si parola. Apoi, trimit un post request la server, primesc raspunsul, afisez output-ul in functie de codul returnat de server si inchid conexiunea.

            -> login: Similar cu register, difera path-ul si faptul ca dupa login extrag cookie-ul folosind functia get_cookie().

            -> enter_library: Setez URL-ul, trimit un get request care contine cookie-ul de sesiune. Primmesc apoi raspunsul de la server, din care extrag JWT token-ul folosind get_token().

            -> get_books: Creez un get-request cu URL-ul corespunzator si afiseaza cartile primite de la server.

            -> get_book: Citesc id-ul cartii de la tastatura, verific daca e numar, apoi deschid conexiunea la server, trimit get_request, primesc raspunsul de la server si afisez un raspuns in functie de acesta.

            -> add_book: Citesc datele cartii de la tastatura. Verific daca pentru campul page_count a fost introdus un numar si daca exista vreun camp gol. Apoi, creez un JSON cu toate aceste date, dupa care trimit un post request la server. Apoi, afisez un raspuns in functie de acesta si inchid conexiunea.

            -> logout: Creez un get request cu URL-ul de logout, afisez raspunsul si sterg cookie-ul si token-ul.

            -> exit: Break pentru a iesi din bucla infinita.
