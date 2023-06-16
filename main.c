#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#define NUM_REQUESTS 1000
#define API_ROUTE "route.com"

int main(){
    
    CURL *curl[NUM_REQUESTS];
    CURLcode res[NUM_REQUESTS];
    CURLM *curl_handle;
    long http_code;

    // Inicialize a biblioteca cURL
    curl_global_init(CURL_GLOBAL_ALL);

    // Crie o manipulador múltiplo
    curl_handle = curl_multi_init();

    // Crie as solicitações
    for (int i = 0; i < NUM_REQUESTS; i++) {
        curl[i] = curl_easy_init();
        if (curl[i]) {

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Authorization: Bearer token");
            
            curl_easy_setopt(curl[i], CURLOPT_URL, API_ROUTE);
            curl_easy_setopt(curl[i], CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl[i], CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curl[i], CURLOPT_HTTPHEADER, headers);
            curl_multi_add_handle(curl_handle, curl[i]);
        }
    }

    int still_running;
    do {
        // Execute as solicitações pendentes
        curl_multi_perform(curl_handle, &still_running);
    } while (still_running);

    int counter = 0;

    // Verifique o status de cada solicitação
    for (int i = 0; i < NUM_REQUESTS; i++) {
        res[i] = curl_easy_getinfo(curl[i], CURLINFO_RESPONSE_CODE, &http_code);
        if (res[i] != CURLE_OK) {
            fprintf(stderr, "Erro ao obter o status da solicitação %d: %s\n", i, curl_easy_strerror(res[i]));
        } else {
            if(http_code == 200){
                counter++;
            }
            printf("Status da solicitação %d: %ld\n", i, http_code);
        }
    }

    // Libere os recursos
    for (int i = 0; i < NUM_REQUESTS; i++) {
        curl_multi_remove_handle(curl_handle, curl[i]);
        curl_easy_cleanup(curl[i]);
    }
    curl_multi_cleanup(curl_handle);
    curl_global_cleanup();

    printf("\nRequests with 200 http code: %d\n\n",counter);

    return 0;
}
