#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"
#include "ctype.h"
#include "math.h"

#define INF 10000

typedef struct _Tile{
	int posX;
	int posY;
	int piso;
	int id;
	struct _Tile* next;
	int idLlave;
	int lifeMonster;
}Tile;

typedef struct _Door{
	int posX1;
	int posY1;
	int posX2;
	int posY2;
	int id;
	int piso;
	struct _Door* next; 
}Door;

int ancho;
int alto;
int numPisos;
int casillasTotal;
int lifeBarbaro;

int** createMatrix();
int getIdNodo(int posX, int posY, int piso);
void printMatriz(int** matrAdy);
int getNodoLlave(Tile* tileList, int id);
void printCaminoMinimo(int* caminoMinimo, int tamCaminoMinimo);
void asignarCaminoMinimo(int* caminoMinimo, int* tamCaminoMin, int* pred, int startNode, int endNode);
void printConclusion(int* caminoMinimo, int distance, Tile* tileList, int haLlegado);
void dijkstra(int** G, int startNode, int finalNode, int* distance, int* pred);

int main(){

	//Lectura de archivo y vida de Hrognan
	printf("Ingresar vidas de Hrognan: \n");
	scanf("%d", &lifeBarbaro);
	
	FILE* input;
	char file_name[20];
	printf("Ingresar nombre de archivo a leer: \n");
	scanf("%s", file_name);
    input= fopen(file_name, "r");
	fscanf(input, "%d", &numPisos);
	fscanf(input, "%d", &ancho);
	fscanf(input, "%d", &alto);

	casillasTotal= alto*ancho*numPisos;

	//Creacion nodos.

	int id= 0;

	Tile* tileListHead= malloc(sizeof(Tile));
	tileListHead->posX= 0;
	tileListHead->posY= 0;
	tileListHead->piso= 0;
	tileListHead->id= id;
	tileListHead->next= NULL;
	tileListHead->idLlave= -1;
	tileListHead->lifeMonster= 0;
	
	Tile* itTile= tileListHead;
	for(int z= 0; z< numPisos; z++){
		for(int j= 0; j< alto; j++){
			for(int i= 0; i< ancho; i++){
				if(i != 0 || j != 0 || z != 0){
					id++;

					Tile* aux= malloc(sizeof(Tile));
					aux->posX= i;
					aux->posY= j;
					aux->piso= z;
					aux->id= id;
					aux->next= NULL;
					aux->idLlave= -1;
					aux->lifeMonster= 0;
	
				
					itTile->next= aux;
					itTile= itTile->next;
				}
			}	
		}
	}


	//Creacion de matriz de adyacencia.
	int** adyMatr= createMatrix();
 
	//Datos barbaro
	int pisoBarbaro; 
	int xBarbaro;
	int yBarbaro;
	fscanf(input, "%d", &pisoBarbaro);
	fscanf(input, "%d", &xBarbaro);
	fscanf(input, "%d", &yBarbaro);

	int nodoBarbaro= getIdNodo(xBarbaro, yBarbaro, pisoBarbaro);

	//Datos salida
	int pisoEnd;
	int xEnd;
	int yEnd;
	int idEnd;
	fscanf(input, "%d", &pisoEnd);
	fscanf(input, "%d", &xEnd);
	fscanf(input, "%d", &yEnd);

	idEnd= getIdNodo(xEnd, yEnd, pisoEnd);

	//Agregación de muros a matriz.
	char line[10]; 
	fscanf(input, "%s", line);
  	int currX1;
  	int currY1;
  	int currX2;
  	int currY2;
  	while(strcmp(line, "puertas") != 0){
  		fscanf(input, "%d", &currX1);
		fscanf(input, "%d", &currY1);
		fscanf(input, "%d", &currX2);
		fscanf(input, "%d", &currY2);
		//Identificar nodos cuyos caminos son bloqueados por muros.
		if(currX1 == currX2){
			int numMuros= abs(currY1-currY2);
			int yMenor;
			if(currY2< currY1) yMenor= currY2;
			else yMenor= currY1;
			for(int i= 0; i< numMuros; i++){
				int node1= getIdNodo(currX1-1, yMenor, atoi(line));
				int node2= getIdNodo(currX1, yMenor, atoi(line));
				
				adyMatr[node1][node2]= 0;
				adyMatr[node2][node1]= 0;

				yMenor++;
			}
		}else if(currY1 == currY2){
			int numMuros= abs(currX1-currX2);
			int xMenor;
			if(currX2< currX1) xMenor= currX2;
			else xMenor= currX1;
			for(int i= 0; i< numMuros; i++){
				int node1= getIdNodo(xMenor, currY1-1, atoi(line));
				int node2= getIdNodo(xMenor, currY1, atoi(line));

				adyMatr[node1][node2]= 0;
				adyMatr[node2][node1]= 0;

				xMenor++;
			}
		}
		fscanf(input, "%s", line);
  	}

  	//Lectura de puertas
	Door* doorHeadList= malloc(sizeof(Door));
	fscanf(input, "%s", line);

	if(strcmp(line, "llaves") != 0){
		doorHeadList->id= atoi(line);
  		fscanf(input ,"%d", &doorHeadList->piso);
  		fscanf(input ,"%d", &doorHeadList->posX1);
  		fscanf(input ,"%d", &doorHeadList->posY1);
  		fscanf(input ,"%d", &doorHeadList->posX2);
  		fscanf(input ,"%d", &doorHeadList->posY2);
  		doorHeadList->next= NULL;

  		fscanf(input ,"%s", line);
	}

  	Door* itDoor= doorHeadList;

  	while(strcmp(line, "llaves") != 0){ //Aqui se guardan los datos de las puertas que serán usados mas adelante.
  		Door* aux= malloc(sizeof(Door));
  		aux->id= atoi(line);
  		fscanf(input ,"%d", &aux->piso);
  		fscanf(input ,"%d", &aux->posX1);
  		fscanf(input ,"%d", &aux->posY1);
  		fscanf(input ,"%d", &aux->posX2);
  		fscanf(input ,"%d", &aux->posY2);
  		aux->next= NULL;

  		itDoor->next= aux;
  		itDoor= itDoor->next;

  		fscanf(input ,"%s", line);
  	}
 	//Lectura de llaves
  	fscanf(input ,"%s", line);
  	while(strcmp(line, "monstruos") != 0){
  		int pisoLlave;
  		int posXLlave;
  		int posYLLave;
  		fscanf(input ,"%d", &pisoLlave);
  		fscanf(input ,"%d", &posXLlave);
  		fscanf(input ,"%d", &posYLLave);
  		int idNodo= getIdNodo(posXLlave, posYLLave, pisoLlave);
  		itTile= tileListHead;
  		for(int i= 0; i< idNodo; i++){
  			itTile= itTile->next;
  		}
  		itTile->idLlave= atoi(line);

  		fscanf(input ,"%s", line);
  	}

  	//Lectura de monstruos

  	fscanf(input ,"%s", line);
  	while(strcmp(line, "portales") != 0){
  		int pisoMonstruo= atoi(line);
  		int posXMonstruo;
  		int posYMonstruo;
  		int vidaMonstruo;
  		fscanf(input ,"%d", &posXMonstruo);
  		fscanf(input ,"%d", &posYMonstruo);
  		fscanf(input ,"%d", &vidaMonstruo);
  		int idNodo= getIdNodo(posXMonstruo, posYMonstruo, pisoMonstruo);
  		itTile= tileListHead;
  		for(int i= 0; i< idNodo; i++){
  			itTile= itTile->next;
  		}
  		itTile->lifeMonster= vidaMonstruo;

  		fscanf(input ,"%s", line);
  	}
 
  	//Lectura de portales
  	fscanf(input ,"%s", line);
  	while(strcmp(line, "escaleras") != 0){
  		int pisoPortal1= atoi(line);
  		int posXPortal1;
  		int posYPortal1;
  		int pisoPortal2;
  		int posXPortal2;
  		int posYPortal2;
  		fscanf(input ,"%d", &posXPortal1);
  		fscanf(input ,"%d", &posYPortal1);
  		fscanf(input ,"%d", &pisoPortal2);
  		fscanf(input ,"%d", &posXPortal2);
  		fscanf(input ,"%d", &posYPortal2);
  		int idNodoPortal1= getIdNodo(posXPortal1, posYPortal1, pisoPortal1);
  		int idNodoPortal2= getIdNodo(posXPortal2, posYPortal2, pisoPortal2);
  		adyMatr[idNodoPortal1][idNodoPortal2]= 1;
  		adyMatr[idNodoPortal2][idNodoPortal1]= 1;

  		fscanf(input ,"%s", line);
  	}

  	//Lectura de escaleras
  	int end= fscanf(input ,"%s", line);
  	while(end!= EOF){
  		int pisoEscalera= atoi(line);
  		int posXEscalera;
  		int posYEscalera;
  		fscanf(input ,"%d", &posXEscalera);
  		fscanf(input ,"%d", &posYEscalera);

  		int idNodoEscalera1= getIdNodo(posXEscalera, posYEscalera, pisoEscalera);
  		int idNodoEscalera2= getIdNodo(posXEscalera, posYEscalera, pisoEscalera+1);
  		adyMatr[idNodoEscalera1][idNodoEscalera2]= 1;
  		adyMatr[idNodoEscalera2][idNodoEscalera1]= 1;

  		end= fscanf(input ,"%s", line);
  	}
  	
  	fclose(input);

  	//--- FIN LECTURA --- 

  	//--- COMIENZO CALCULO CAMINO MINIMO ---


  	int* distance= (int*)malloc(casillasTotal*sizeof(int));
  	int* pred= (int*)malloc(casillasTotal*sizeof(int));
  	int* idDoorsPassed= (int*)malloc(casillasTotal*2*sizeof(int));
  	int* nodosLlaves= (int*)malloc(casillasTotal*2*sizeof(int));
  	int* caminoMinimo= (int*)malloc(casillasTotal*sizeof(int));

  	/*Impresion matriz de adyacencia con las puertas abiertas.
  	printf("Matriz de adyacencia con puertas abiertas: \n");
  	printMatriz(adyMatr);
  	*/

  	//Agregar puertas que no tienen llave como puertas cerradas:
  	//1-Comparar cantidad e ids de puertas existentes.
  	int* idDoorswithKeyAvailable= (int*)malloc(casillasTotal*2*sizeof(int));
  	int numDoorswithKey= 0;
  	itDoor= doorHeadList;
  	while(itDoor!= NULL){
  		int aux= getNodoLlave(tileListHead, itDoor->id);
  		if(aux != -1){
  			idDoorswithKeyAvailable[numDoorswithKey]= itDoor->id;
  			numDoorswithKey++;
  		}

  		itDoor= itDoor->next;
  	}  	

  	itDoor= doorHeadList;
  	while(itDoor != NULL){
  		int i= 0; 
		int flagDoor= 0;
		while(i < numDoorswithKey){
			if(itDoor->id == idDoorswithKeyAvailable[i]){
				flagDoor= 1;
				break;
			}
			i++;
		}	
		if(!flagDoor){
  			if(itDoor->posX1 == itDoor->posX2){
				int numMuros= abs(itDoor->posY1 - itDoor->posY2);
				int yMenor;
				if(itDoor->posY2< itDoor->posY1) yMenor= itDoor->posY2;
				else yMenor= itDoor->posY1;

				for(int i= 0; i< numMuros; i++){
					int node1= getIdNodo(itDoor->posX1-1, yMenor, itDoor->piso);
					int node2= getIdNodo(itDoor->posX1, yMenor, itDoor->piso);

					adyMatr[node1][node2]= 0;
					adyMatr[node2][node1]= 0;
				
					yMenor++;
				}
			}else if(itDoor->posY1 == itDoor->posY2){
				int numMuros= abs(itDoor->posX1 - itDoor->posX2);
				int xMenor;
				if(itDoor->posX2< itDoor->posX1) xMenor= itDoor->posX2;
				else xMenor= itDoor->posX1;
				for(int i= 0; i< numMuros; i++){
					int node1= getIdNodo(xMenor, itDoor->posY1-1, itDoor->piso);
					int node2= getIdNodo(xMenor, itDoor->posY1, itDoor->piso);
					
					adyMatr[node1][node2]= 0;
					adyMatr[node2][node1]= 0;
					
					xMenor++;
				}
			}
		}
		itDoor= itDoor->next;
  	}
  	//djikstra para matriz.
  	dijkstra(adyMatr, nodoBarbaro, idEnd, distance, pred);

  	//Llenado arreglo de camino minimo con el recorrido hecho para el camino minimo.
  	int tamCaminoMin;
  	asignarCaminoMinimo(caminoMinimo, &tamCaminoMin, pred, nodoBarbaro, idEnd);
  	
  	
  	

  	
  	//Asignar ids de puertas a matriz de adyacencia para comprobar que puertas han sido atravesadas
  	//en arreglo pred.
  	
  	itDoor= doorHeadList;
  	while(itDoor != NULL){
  		if(itDoor->posX1 == itDoor->posX2){
			int numMuros= abs(itDoor->posY1 - itDoor->posY2);
			int yMenor;
			if(itDoor->posY2< itDoor->posY1) yMenor= itDoor->posY2;
			else yMenor= itDoor->posY1;

			for(int i= 0; i< numMuros; i++){
				int node1= getIdNodo(itDoor->posX1-1, yMenor, itDoor->piso);
				int node2= getIdNodo(itDoor->posX1, yMenor, itDoor->piso);
				
				adyMatr[node1][node2]= itDoor->id+2;
				adyMatr[node2][node1]= itDoor->id+2;
					
				yMenor++;
			}
		}else if(itDoor->posY1 == itDoor->posY2){
			int numMuros= abs(itDoor->posX1 - itDoor->posX2);
			int xMenor;
			if(itDoor->posX2< itDoor->posX1) xMenor= itDoor->posX2;
			else xMenor= itDoor->posX1;
			for(int i= 0; i< numMuros; i++){
				int node1= getIdNodo(xMenor, itDoor->posY1-1, itDoor->piso);
				int node2= getIdNodo(xMenor, itDoor->posY1, itDoor->piso);
				
				adyMatr[node1][node2]= itDoor->id+2;
				adyMatr[node2][node1]= itDoor->id+2;
				
				xMenor++;
			}
		}
		itDoor= itDoor->next;
  	}

  	//Asignar ids a arreglo de puertas atravesadas. 
  	int nodoActual= nodoBarbaro;
  	int numPuertas= 0;
  	for(int i= tamCaminoMin-1; i>= 0; i--){
  		int arista= adyMatr[ caminoMinimo[i] ][ caminoMinimo[i+1] ];
  		if(arista != 0 && arista != 1){
  			idDoorsPassed[numPuertas]= arista-2;
  			numPuertas++;
  		}
  	}

  	//Encontrar nodos de llaves existentes correspondientes a puertas atravesadas en camino minimo.  
  	int numLlaves= 0;
  	for(int i= 0; i< numPuertas; i++){
  		int aux= getNodoLlave(tileListHead, idDoorsPassed[i]);
  		if(aux != -1){
  			nodosLlaves[numLlaves]= aux;
  			numLlaves++; 
  		}
  	}

  	//Cerrar puertas para calcular camino minimo considerando las llaves
  	itDoor= doorHeadList;
  	while(itDoor != NULL){
  		if(itDoor->posX1 == itDoor->posX2){
			int numMuros= abs(itDoor->posY1 - itDoor->posY2);
			int yMenor;
			if(itDoor->posY2< itDoor->posY1) yMenor= itDoor->posY2;
			else yMenor= itDoor->posY1;

			for(int i= 0; i< numMuros; i++){
				int node1= getIdNodo(itDoor->posX1-1, yMenor, itDoor->piso);
				int node2= getIdNodo(itDoor->posX1, yMenor, itDoor->piso);
				
				adyMatr[node1][node2]= 0;
				adyMatr[node2][node1]= 0;
					
				yMenor++;
			}
		}else if(itDoor->posY1 == itDoor->posY2){
			int numMuros= abs(itDoor->posX1 - itDoor->posX2);
			int xMenor;
			if(itDoor->posX2< itDoor->posX1) xMenor= itDoor->posX2;
			else xMenor= itDoor->posX1;
			for(int i= 0; i< numMuros; i++){
				int node1= getIdNodo(xMenor, itDoor->posY1-1, itDoor->piso);
				int node2= getIdNodo(xMenor, itDoor->posY1, itDoor->piso);
				
				adyMatr[node1][node2]= 0;
				adyMatr[node2][node1]= 0;
				
				xMenor++;
			}
		}
		itDoor= itDoor->next;
  	}

  	//Comienzo calculo de caminos minimos de llaves, junto a una matriz de caminos minimos que guardara todos los 
  	//caminos minimos para cada llave.
  	int** distanciasLlaves= (int**)malloc(casillasTotal*sizeof(int*));
  	for(int i= 0; i< casillasTotal; i++){
  		distanciasLlaves[i]= (int*)malloc(casillasTotal*sizeof(int));
  	}
  	int** predLlaves= (int**)malloc(casillasTotal*sizeof(int*));
  	for(int i= 0; i< casillasTotal; i++){
  		predLlaves[i]= (int*)malloc(casillasTotal*sizeof(int));
  	}
  	int** caminoMinimoLlaves= (int**)malloc(casillasTotal*sizeof(int*));
  	for(int i= 0; i< casillasTotal; i++){
  		caminoMinimoLlaves[i]= (int*)malloc(casillasTotal*sizeof(int));
  	}
  	int* tamCaminoMinLlaves= (int*)malloc(casillasTotal*sizeof(int));

  	int currentStartNode= nodoBarbaro;
  	int currentEndNode;
  	for(int i= 0; i< numLlaves; i++){
  		currentEndNode= nodosLlaves[i];
  		dijkstra(adyMatr, currentStartNode, currentEndNode, distanciasLlaves[0], predLlaves[0]);
  		asignarCaminoMinimo(caminoMinimoLlaves[i], &tamCaminoMinLlaves[i], predLlaves[i], currentStartNode, currentEndNode);
  		//Eliminacion puerta al conseguir llave de currentEndNode:
  		itDoor= doorHeadList;
  		itTile= tileListHead;
  		for(int j= 0; j< nodosLlaves[i]; j++){ //Llegar a nodo que contiene la id de la llave para comparar con id de puerta
  			itTile= itTile->next;
  		}

  		while(itDoor != NULL){
  			if(itDoor->id == itTile->idLlave ){
  				if(itDoor->posX1 == itDoor->posX2){
					int numMuros= abs(itDoor->posY1 - itDoor->posY2);
					int yMenor;
					if(itDoor->posY2< itDoor->posY1) yMenor= itDoor->posY2;
					else yMenor= itDoor->posY1;

					for(int z= 0; z< numMuros; z++){
						int node1= getIdNodo(itDoor->posX1-1, yMenor, itDoor->piso);
						int node2= getIdNodo(itDoor->posX1, yMenor, itDoor->piso);
						
						adyMatr[node1][node2]= 1;
						adyMatr[node2][node1]= 1;
					
						yMenor++;
					}
				}else if(itDoor->posY1 == itDoor->posY2){
					int numMuros= abs(itDoor->posX1 - itDoor->posX2);
					int xMenor;
					if(itDoor->posX2< itDoor->posX1) xMenor= itDoor->posX2;
					else xMenor= itDoor->posX1;
					for(int z= 0; z< numMuros; z++){
						int node1= getIdNodo(xMenor, itDoor->posY1-1, itDoor->piso);
						int node2= getIdNodo(xMenor, itDoor->posY1, itDoor->piso);
				
						adyMatr[node1][node2]= 1;
						adyMatr[node2][node1]= 1;
				
						xMenor++;
					}
				}
  			}
  			itDoor= itDoor->next;
  		}
  		currentStartNode= nodosLlaves[i];
  	}
  	//Camino minimo desde la ultima llave a la salida.
  	int* distanciasLlaveFinal= (int*)malloc(casillasTotal*sizeof(int));
  	int* predLlaveFinal= (int*)malloc(casillasTotal*sizeof(int));
  	int* caminoMinimoLlaveFinal= (int*)malloc(casillasTotal*sizeof(int));
  	int tamCaminoLlaveFinal;

  	dijkstra(adyMatr, currentStartNode, idEnd, distanciasLlaveFinal, predLlaveFinal);
  	asignarCaminoMinimo(caminoMinimoLlaveFinal, &tamCaminoLlaveFinal, predLlaveFinal, currentStartNode, idEnd);
  	
  	//Check si es que es posible llegar a la meta.	
  	int haLlegado= 0;
  	if(distanciasLlaveFinal[idEnd] != INF){
  		haLlegado= 1;
  	}

  	//Este es el camino minimo final que consiste en la suma de todos los caminos minimos entre llaves  y el camino minimo hasta el final.
  	int* caminoMinimoFinal= (int*)malloc(casillasTotal*sizeof(int));
  	for(int i= 0; i< tamCaminoLlaveFinal; i++){
  		caminoMinimoFinal[i]= caminoMinimoLlaveFinal[i];
  	}
  	int aux= tamCaminoLlaveFinal;
  	int caminoMinimoFinalTam= aux;
  	for(int i= 0; i< numLlaves; i++){
  		for(int j= 1; j< tamCaminoMinLlaves[i]; j++){
  			caminoMinimoFinal[aux]= caminoMinimoLlaves[i][j];  
  			aux++; 
  			caminoMinimoFinalTam++;
  		}
  	}
  	int distanceFinal= caminoMinimoFinalTam-1;

  	printConclusion(caminoMinimoFinal, distanceFinal, tileListHead, haLlegado);

	return 0;
}



int** createMatrix(){
	//Asignación de espacio a matriz.
	int** matrAdy= (int**)malloc(casillasTotal*sizeof(int*));
	for(int i= 0; i< casillasTotal; i++){
		matrAdy[i]= (int*)malloc(casillasTotal*sizeof(int));
	}	

	//Inicializar en 0 las casillas de la matriz.
	for(int i= 0; i< casillasTotal; i++){
		for(int j= 0; j< casillasTotal; j++){
			matrAdy[i][j]= 0;
		}
	}
	int anchoActual= ancho;
	int altoActual= alto;
	int pisoActual= ancho*alto;

	//Enlazar casillas adyacentes.

	for(int z= 0; z < numPisos; z++){
		for(int j= 0; j< alto; j++){
			for(int i= 0; i < ancho; i++){

				int idNodo= getIdNodo(i, j, z);
				int idNodo2;
				if( i != 0 ){
					idNodo2= getIdNodo(i-1,j,z); 
					matrAdy[idNodo][idNodo2]= 1;
					matrAdy[idNodo2][idNodo]= 1;
				}
				if(i != ancho-1){
					idNodo2= getIdNodo(i+1,j,z);
					matrAdy[idNodo][idNodo2]= 1;
					matrAdy[idNodo2][idNodo]= 1;
				}
				if(j != 0){ 
					idNodo2= getIdNodo(i,j-1,z);
					matrAdy[idNodo][idNodo2]= 1;
					matrAdy[idNodo2][idNodo]= 1;
				}
				if(j != alto-1){ 
					idNodo2= getIdNodo(i,j+1,z);
					matrAdy[idNodo][idNodo2]= 1;
					matrAdy[idNodo2][idNodo]= 1;
				}
			}
		}
	}
	return matrAdy;
}

int getIdNodo(int posX, int posY, int piso){
	int id= (posX+posY)+(ancho-1)*posY+(ancho*alto*piso); 
	return id;
}

void printMatriz(int** matrAdy){
	int num= 0;
	printf(" ");
	for(int i= 0; i< casillasTotal; i++){
		if(num == 10) num= 0;
		printf(" %d", num );
		num++;
	}
	num= 0;
	printf("\n");
	for(int i= 0; i< casillasTotal; i++){
		if(num == 10) num= 0;
		printf("%d ", num);
		num++;
		for(int j= 0; j< casillasTotal; j++){
			printf("%d ", matrAdy[i][j] );
		}
		printf("\n");
	}
}

void dijkstra(int** G, int startNode, int finalNode, int* distance, int* pred){
	int cost[casillasTotal][casillasTotal];
	int visited[casillasTotal], count, minDistance, nextNode, i, j;
	
	//pred guarda el predecesor de cada nodo
	//count da el numero de nodos visitados
	//Crear la matriz de costes
	for(i= 0;i< casillasTotal; i++)
		for(j= 0;j< casillasTotal; j++)
			if(G[i][j]== 0)
				cost[i][j]= INF;
			else
				cost[i][j]= G[i][j];	
	
	//Inicializar pred, distance, visited.
	for(i= 0;i< casillasTotal; i++){
		//Si se encuentra una puerta al lado del nodo inicial, la id de esta puerta se guarda en el arreglo de puertas 
		//atravesadas, y se asigna un valor INF a la distancia del nodo inicial a ese nodo porque hay una puerta de por
		//medio.
		distance[i]= cost[startNode][i];
		pred[i]= startNode;
		visited[i]= 0;
	}
	
	distance[startNode]=0;	
	visited[startNode]=1;
	count=1;

	
	while(count< casillasTotal-1){
		minDistance= INF;
		
		//nextNode es el nodo a distancia minima, el cual es obtenido despues de comparar la distancia minima actual con la distancia al nodo
		for(i= 0;i< casillasTotal; i++){
			if(distance[i]< minDistance && !visited[i]){
				minDistance= distance[i];
				nextNode= i;
			}
		} 
		
		//Checkea si existe un mejor camino através de nextNode, es decir, compara con los nodos aun no visitados la distancia.		
		visited[nextNode]= 1;
		for(i= 0; i< casillasTotal; i++){
			if(!visited[i]){
				if(minDistance + cost[nextNode][i] < distance[i]){
					distance[i]= minDistance + cost[nextNode][i];
					pred[i]= nextNode;
				}
			}
		}
		count++;

		
	}
	//Imprimir el camino y distancia hacia cada nodo
	/*for(i=0;i<n;i++)
		if(i!=startnode)
		{
			printf("\nDistance of node%d=%d",i,distance[i]);
			printf("\nPath=%d",i);
			
			j=i;
			do
			{
				j=pred[j];
				printf("<-%d",j);
			}while(j!=startnode);
	}*/


	
}

int getNodoLlave(Tile* tileList, int id){
	Tile* itTile= tileList;
	while(itTile != NULL){
		if(itTile->idLlave == id){
			return itTile->id;
		}
		itTile= itTile->next; 
	}
	return -1;
}

void printCaminoMinimo(int* caminoMinimo, int tamCaminoMin){	
  	for(int i= 0; i< tamCaminoMin; i++){
  		printf("%d ", caminoMinimo[i]);
  	}
  	printf("\n");
}

void asignarCaminoMinimo(int* caminoMinimo, int* tamCaminoMin, int* pred, int startNode, int endNode){
	int aux1= endNode, aux2= 1;
  	caminoMinimo[0]= aux1;
  	while(aux1!= startNode){
  		aux1= pred[aux1];
  		caminoMinimo[aux2]= aux1;
  		aux2++;	
  	}
  	*tamCaminoMin= aux2;
}

void printConclusion(int* caminoMinimo, int distance, Tile* tileList, int haLlegado){
	Tile* itTile= tileList;
	if(haLlegado){
		printf("El camino mínimo que debe seguir Hrognan toma %d movimientos en llegar a la salida. \n", distance);
		for(int i= distance; i>= 0; i--){
			itTile= tileList;
			while(itTile!=NULL){
				if(itTile->id == caminoMinimo[i]){
					printf("[%d, %d, %d] \n", itTile->piso, itTile->posX, itTile->posY);
					break;
				}else{
					itTile= itTile->next;
				}
			}
		}
	}else{
		printf("Hrognan no ha podido llegar a la salida. \n ");
	}
}

