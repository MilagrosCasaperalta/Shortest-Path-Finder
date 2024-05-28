#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct stazione {
    int distanza;
    int maxautonomia;
    struct stazione *sucminore;
    struct stazione *sucmaggiore;
    struct stazione *padre;
    int* macchine;
};

struct stazione *root;

struct stazione *cerca_stazione(int dist) {
    struct stazione *temp = root;
    if(dist == temp -> distanza){
        return temp;
    }

    while (temp != NULL) {
        if (dist < temp->distanza) {
            temp = temp->sucminore;
        } else if (dist > temp->distanza) {
            temp = temp->sucmaggiore;
        } else if (dist == temp->distanza) {
            return temp;
        }
    }
    return temp;
}

int cerca_maxautonomia(struct stazione* t);

int rottama_auto(int dist, int autonomia){

    struct stazione* distanza = cerca_stazione(dist);

    if(distanza != NULL) {
        int* p = distanza->macchine;
        int i = 0;
        while (i < 512 && *(p+i) != autonomia) {
            i++;
        }
        if (*(p+i) == autonomia) {
            *(p+i) = -1;
            printf("rottamata\n");
            distanza->maxautonomia = cerca_maxautonomia(distanza);
            return 1;
        } else {
            printf("non rottamata\n");
            return 1;
        }
    }else{
        printf("non rottamata\n");
        return 1;
    }
}

struct stazione *cerca_succ(struct stazione *partenza, int verso){


    struct stazione *p = partenza;

    if (verso == 1) {
        if (partenza->sucmaggiore != NULL) {
            p = partenza->sucmaggiore;
            while (p->sucminore != NULL) {
                p = p->sucminore;
            }

            return p;
        }
        p = partenza->padre;
        while (p != NULL && partenza == p->sucmaggiore) {
            partenza = p;
            p = p->padre;
        }
        return p;

    } else if (verso == -1) {
        printf("inutile\n");
    }
    return p;
}

struct stazione* cercaprec(int val);




struct stazione *cerco_max(struct stazione *partenza, int direzione, int max) {
    struct stazione *p = partenza;
    struct stazione *m = p;

    if (direzione == 1) {

        while (p->distanza <= max) {
            m = p;
            p = cerca_succ(p, 1);
            if (p == NULL) {
                return m;
            }
        }
        if (p->distanza > max) {
            return m;
        } else {
            return p;
        }


    } else if (direzione == -1) {

        int i = 0;

        while (p->distanza >= max) {
            m = p;
            i = p->distanza;
            p = cercaprec(i);

            if (p == NULL || p->distanza < max) {
                return m;
            }

        }
        return m;
    }
    return m;
}


int cancellaStazione(int k) {

    struct stazione* curr = root;
    struct stazione* prev = NULL;

    if(root->distanza == k && root->sucmaggiore == NULL && root->sucminore == NULL){
        free(root->macchine);
        free(root);
        root = NULL;
        printf("demolita\n");
        return 1;
    }

    while (curr != NULL && curr->distanza != k) {
        prev = curr;
        if (k < curr->distanza)
            curr = curr->sucminore;
        else
            curr = curr->sucmaggiore;
    }

    if (curr == NULL) {
        printf("non demolita\n");
        return 2;
    }

    if(curr->sucmaggiore == NULL && curr->sucminore == NULL){
        //SE HO UNA FOGLIA
        if(prev->sucminore == curr){
            prev->sucminore = curr->sucmaggiore;
        }else{
            prev->sucmaggiore = curr->sucmaggiore;
        }
        free(curr->macchine);
        free(curr);
        printf("demolita\n");
        return 3;
    }

    if (curr->sucminore == NULL || curr->sucmaggiore == NULL) {

        struct stazione *newCurr;

        if (curr->sucminore == NULL)
            newCurr = curr->sucmaggiore;
        else
            newCurr = curr->sucminore;

        if (prev == NULL){
            free(root->macchine);
            free(root);
            root = newCurr;
            newCurr -> padre = NULL;
            printf("demolita\n");
            return 1;
        }
        if (curr == prev->sucminore)
            prev->sucminore = newCurr;
        else
            prev->sucmaggiore = newCurr;

        newCurr->padre = curr->padre;

        free(curr->macchine);
        free(curr);
        curr = NULL;

        printf("demolita\n");

        return 5;
    }

    else {
        //SE HO ENTRAMBI I FIGLI
        struct stazione *p = NULL;

        struct stazione *s= curr->sucmaggiore;

        while (s->sucminore != NULL) {
            p = s;
            s = s->sucminore;
        }
        if (p != NULL) {
            p->sucminore = s->sucmaggiore;
            if(s->sucmaggiore != NULL) {
                s->sucmaggiore->padre = p;
            }
        }
        else {
            curr->sucmaggiore = s->sucmaggiore;
            if(curr->sucmaggiore != NULL) {
                curr->sucmaggiore->padre = curr;
            }
        }
        curr->maxautonomia = s->maxautonomia;
        curr->distanza = s->distanza;
        free(curr->macchine);
        curr->macchine = s->macchine;
        free(s);
        s = NULL;

        printf("demolita\n");
        return 6;

    }

    return 1;
}

struct stazione *inizia_stazione(int k) {
    struct stazione *t = (struct stazione *) malloc(sizeof(struct stazione));
    t->distanza = k;
    t->sucmaggiore = NULL;
    t->sucminore = NULL;
    t->padre = NULL;
    t->macchine = (int*)malloc(sizeof(int)*512);
    t->maxautonomia = 0;
    return t;
}

int cerca_maxautonomia(struct stazione* t){

    int* temp = t->macchine;


    int max = 0;
    int i = 0;
    while(i < 512){
        if( *(temp+i) > max){
            max = *(temp+i);
        }
        i++;
    }
    return max;
}


int creaStazione(int distanzaInt) {
    FILE *file;
    file = stdin;
    struct stazione *tempStazione;
    struct stazione *nuovaStazione;

    if (root != NULL) {
        if (root->distanza == distanzaInt) {
            return 2;
        } else {

            if(root == NULL){
                return -1;
            }else
                tempStazione = root;

            nuovaStazione = inizia_stazione(distanzaInt);

            while (tempStazione != NULL && tempStazione->distanza != distanzaInt) {
                if (tempStazione->distanza < distanzaInt) {
                    if (tempStazione->sucmaggiore == NULL) {
                        tempStazione->sucmaggiore = nuovaStazione;
                        tempStazione->sucmaggiore->padre = tempStazione;
                    } else {
                        tempStazione = tempStazione->sucmaggiore;
                    }
                } else if (tempStazione->distanza > distanzaInt) {
                    if (tempStazione->sucminore == NULL) {
                        tempStazione->sucminore = nuovaStazione;
                        tempStazione->sucminore->padre = tempStazione;
                    } else {
                        tempStazione = tempStazione->sucminore;
                    }
                }
            }
            if (tempStazione->distanza == distanzaInt && tempStazione != nuovaStazione) {
                free(nuovaStazione);
                return 2;
            }

        }
    } else {

        root = inizia_stazione(distanzaInt);
        tempStazione = root;

    }



    char ripetizioniChar[10];
    int ripetizioniInt = 0;
    int *tempMacchina = tempStazione->macchine;
    char autonomiaChar[15];
    int autonomiaInt = 0;


    if( fscanf(file, "%s", ripetizioniChar) != 1){
        printf("errore\n");
        return 1;
    }
    ripetizioniInt = atoi(ripetizioniChar);
    int i = 0;

    while(i < 512){
        tempMacchina[i] = -1;
        i++;
    }
    i = 0;


    if(ripetizioniInt != 0) {
        while (ripetizioniInt > 0) {

            if (fscanf(file, "%s", autonomiaChar) != 1) {
                printf("errore\n");
                return 1;
            }



            autonomiaInt = atoi(autonomiaChar);
            tempMacchina[i] = autonomiaInt;
            i++;
            ripetizioniInt--;

        }
        tempStazione->maxautonomia = cerca_maxautonomia(tempStazione);
    }


    return 0;
}


int aggiungi_auto(int stazione, int autonomia){

    struct stazione* staz = cerca_stazione(stazione);
    if(staz != NULL){
        int i = 0;
        while(i < 512){
            if(staz->macchine[i] == -1){

                staz->macchine[i] = autonomia;

                staz->maxautonomia = cerca_maxautonomia(staz);
                printf("aggiunta\n");

                return 1;

            }else{
                i++;
            }
        }

    }else{
        printf("non aggiunta\n");
    }

    return -1;
}


struct tappa {

    int dist;
    struct tappa *succ;
    struct tappa *prec;

};

struct tappa *crea_tappa(int distanza) {

    struct tappa *new = (struct tappa *) malloc(sizeof(struct tappa));
    new->dist = distanza;
    new->succ = NULL;
    new->prec = NULL;

    return new;

}


struct stazione* cercomin(struct stazione* pas){
    while(pas->sucmaggiore !=NULL){
        pas = pas->sucmaggiore;
    }
    return pas;
}


struct stazione* cercaprec(int val){

    struct stazione* r = root;
    struct stazione* pre = NULL;
    while(r !=NULL){
        if(val < r->distanza){
            r = r->sucminore;
        }  else if(val > r->distanza){
            pre = r;
            r = r->sucmaggiore;
        }else{
            if(r->sucminore != NULL){
                pre = cercomin(r->sucminore);
            }

            return pre;
        }
    }

    return r;
}


int pianifica_percorso(int partenza, int arrivo) {
    struct stazione *massimaDist;
    int maxlontananza = -1;
    int maxTratto = -1;

    if (partenza < arrivo) {

        struct stazione *stazPartenza = cerca_stazione(partenza);
        struct stazione *stazArrivo = cerca_stazione(arrivo);
        struct stazione *esplorazione = stazPartenza;


        struct tappa *percorsoTesta = crea_tappa(stazPartenza->distanza);

        maxlontananza = esplorazione->distanza + esplorazione->maxautonomia;
        massimaDist = cerco_max(esplorazione, 1, maxlontananza);

        if (massimaDist == esplorazione && esplorazione != stazArrivo) {
            printf("nessun percorso\n");
            struct tappa *temp = percorsoTesta;
            while (percorsoTesta->prec != NULL) {
                percorsoTesta = percorsoTesta->prec;
                free(temp);
            }
            percorsoTesta = percorsoTesta->prec;
            free(temp);
            return 1;
        }


        while (massimaDist != stazArrivo) {

            if (esplorazione != NULL) {
                while (massimaDist != esplorazione) {

                    esplorazione = cerca_succ(esplorazione,
                                              1);

                    if (esplorazione->distanza + esplorazione->maxautonomia >= maxlontananza &&

                        esplorazione->distanza + esplorazione->maxautonomia > maxTratto) {

                        maxTratto = esplorazione->distanza +
                                    esplorazione->maxautonomia;


                    }
                }
            }

            struct tappa *nuovaTappa = crea_tappa(
                    massimaDist->distanza);
            while (percorsoTesta->succ !=
                   NULL) {
                percorsoTesta = percorsoTesta->succ;
            }

            percorsoTesta->succ = nuovaTappa;
            nuovaTappa->prec = percorsoTesta;
            percorsoTesta = percorsoTesta->succ;


            massimaDist = cerco_max(esplorazione, 1, maxTratto);

            if(massimaDist->distanza >= stazArrivo->distanza){
                massimaDist = stazArrivo;
            }

            if(massimaDist == esplorazione && esplorazione != stazArrivo){
                printf("nessun percorso\n");
                return 1;
            }
        }


        struct tappa *nuovaTappa = crea_tappa(massimaDist->distanza);
        while (percorsoTesta->succ != NULL) {
            percorsoTesta = percorsoTesta->succ;
        }
        percorsoTesta->succ = nuovaTappa;
        nuovaTappa->prec = percorsoTesta;
        percorsoTesta = percorsoTesta->succ;   //CREATA LA LISTA DEI PERCORSI E PUNTO ALLA STAZIONE PARTENZA


        int primaStaz = 0;

        percorsoTesta = percorsoTesta->prec;
        esplorazione = cerco_max(stazArrivo, -1, percorsoTesta->dist);

        while (percorsoTesta != NULL && esplorazione != stazPartenza) {

            primaStaz = esplorazione->distanza;

            while (esplorazione->distanza <= percorsoTesta->succ->dist &&
                   esplorazione->distanza > percorsoTesta->prec->dist) {


                if (esplorazione->distanza + esplorazione->maxautonomia >= percorsoTesta->succ->dist) {

                    if (primaStaz > esplorazione->distanza) {

                        primaStaz = esplorazione->distanza;
                    }

                }

                int num = esplorazione->distanza;
                esplorazione = cercaprec(num);
                //cerca_succ(esplorazione, -1);

            }

            percorsoTesta->dist = primaStaz;

            percorsoTesta = percorsoTesta->prec;

        }

        while (percorsoTesta->succ != NULL) {
            printf("%d ", percorsoTesta->dist);

            percorsoTesta = percorsoTesta->succ;
            //if(percorsoTesta->prec != NULL){
            //    free(percorsoTesta->prec);
            //}
            if (percorsoTesta->succ == NULL) {
                printf("%d\n", percorsoTesta->dist);
                //    free(percorsoTesta);
            }
        }

        return 1;
    } else if (partenza > arrivo) {

        struct stazione *stazPartenza = cerca_stazione(partenza);  //punto a inizio e fine
        struct stazione *stazArrivo = cerca_stazione(arrivo);
        struct stazione *esplorazione = stazPartenza;

        struct tappa *percorsoTesta = crea_tappa(stazPartenza->distanza);   //INSERISCO LA PRIMA STAZIONE
        maxlontananza = esplorazione->distanza - esplorazione->maxautonomia;
        if(maxlontananza <= stazArrivo->distanza ){
            maxlontananza = stazArrivo->distanza;
        }
        massimaDist = cerco_max(esplorazione, -1, maxlontananza);

        if (massimaDist == esplorazione && esplorazione != stazArrivo) {
            printf("nessun percorso\n");
            struct tappa *temp = percorsoTesta;
            while (percorsoTesta->prec != NULL) {
                percorsoTesta = percorsoTesta->prec;
                free(temp);
            }
            percorsoTesta = percorsoTesta->prec;
            free(temp);
            return 1;
        }

        maxTratto = maxlontananza;

        while (massimaDist != stazArrivo) {
            if (esplorazione != NULL) {
                while (massimaDist != esplorazione) {

                    int i = esplorazione->distanza;

                    esplorazione = cercaprec(i);
                    if (esplorazione->distanza - esplorazione->maxautonomia <= maxlontananza &&
                        esplorazione->distanza - esplorazione->maxautonomia < maxTratto) {
                        maxTratto = esplorazione->distanza - esplorazione->maxautonomia;
                    }
                }
            }

            struct tappa *nuovaTappa = crea_tappa(massimaDist->distanza);
            while (percorsoTesta->succ != NULL) {
                percorsoTesta = percorsoTesta->succ;
            }
            percorsoTesta->succ = nuovaTappa;
            nuovaTappa->prec = percorsoTesta;
            percorsoTesta = percorsoTesta->succ;
            if(maxTratto < stazArrivo->distanza){
                maxTratto = stazArrivo->distanza;
            }
            massimaDist = cerco_max(esplorazione, -1, maxTratto);

            if(massimaDist == esplorazione && esplorazione != stazArrivo){
                printf("nessun percorso\n");
                return 1;
            }
        }
        struct tappa *nuovaTappa = crea_tappa(massimaDist->distanza);
        while (percorsoTesta->succ != NULL) {
            percorsoTesta = percorsoTesta->succ;
        }
        percorsoTesta->succ = nuovaTappa;
        nuovaTappa->prec = percorsoTesta;
        percorsoTesta = percorsoTesta->succ;

        int primaStaz = 0;

        percorsoTesta = percorsoTesta->prec;
        esplorazione = cerco_max(stazArrivo, 1, percorsoTesta->dist);
        while (percorsoTesta != NULL && esplorazione != stazPartenza) {

            int i = 0;

            while (esplorazione->distanza > percorsoTesta->succ->dist &&
                   esplorazione->distanza < percorsoTesta->prec->dist) {

                if (esplorazione->distanza - esplorazione->maxautonomia <= percorsoTesta->succ->dist && i == 0) {
                    primaStaz = esplorazione->distanza;
                    i = 1;
                }

                if(i == 1){
                    esplorazione = cerca_stazione(percorsoTesta->prec->dist);
                } else {
                    esplorazione = cerca_succ(esplorazione, 1);
                }
            }
            percorsoTesta->dist = primaStaz;
            percorsoTesta = percorsoTesta->prec;
        }
        while (percorsoTesta->succ != NULL) {
            printf("%d ", percorsoTesta->dist);
            percorsoTesta = percorsoTesta->succ;
            //if(percorsoTesta->prec != NULL){
            //    free(percorsoTesta->prec);
            //}
            if (percorsoTesta->succ == NULL) {
                printf("%d\n", percorsoTesta->dist);
                //    free(percorsoTesta);
            }
        }
    } else if (partenza == arrivo) {
        printf("%d\n", partenza);
    }
    return 0;
}

int main() {
    FILE *file = stdin;
    char comando[20];
    char distanza[20];
    char autonomia[15];
    int r = 0;

    int distanzaInt;
    int autonomiaInt;

    while (fscanf(file, "%s", comando) != EOF) {


        r = 0;

        if (strcmp(comando, "aggiungi-stazione") == 0) {

            if (fscanf(file, "%s", distanza) != 1) {
                printf("errore\n");
            }
            distanzaInt = atoi(distanza);
            r = creaStazione(distanzaInt);
            if (r == 2) {
                printf("non aggiunta\n");
            } else {
                printf("aggiunta\n");
            }

        } else if (strcmp(comando, "demolisci-stazione") == 0) {  //DA FARE

            if (fscanf(file, "%s", distanza) != 1) {
                printf("errore\n");
                return 1;
            }
            distanzaInt = atoi(distanza);
            cancellaStazione(distanzaInt);

        } else if (strcmp(comando, "aggiungi-auto") == 0) {

            if (fscanf(file, "%s", distanza) != 1) {
                printf("errore\n");
                return 1;
            }
            if (fscanf(file, "%s", autonomia) != 1) {
                printf("errore\n");
                return 1;
            }
            distanzaInt = atoi(distanza);
            autonomiaInt = atoi(autonomia);
            aggiungi_auto(distanzaInt, autonomiaInt);

        } else if (strcmp(comando, "rottama-auto") == 0) {

            if (fscanf(file, "%s", distanza) != 1) {
                printf("errore\n");
                return 1;
            }

            if (fscanf(file, "%s", autonomia) != 1) {
                printf("errore\n");
                return 1;
            }

            distanzaInt = atoi(distanza);
            autonomiaInt = atoi(autonomia);
            rottama_auto(distanzaInt, autonomiaInt);

        } else if (strcmp(comando, "pianifica-percorso") == 0) {
            char inizio[10];
            char fine[10];
            if (fscanf(file, "%s", inizio) != 1) {
                printf("errore\n");
                return 1;
            }

            int inizioInt = atoi(inizio);

            if (fscanf(file, "%s", fine) != 1) {
                printf("errore\n");
                return 1;
            }

            int fineInt = atoi(fine);

            pianifica_percorso(inizioInt, fineInt);
        }
    }

    return 0;
}