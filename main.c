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

void dijkstra(int** G, int n, int startNode, int finalNode, int* distance, int* pred);


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
	tileListHead->id= id;
	tileListHead->next= NULL;
	tileListHead->idLlave= -1;
	tileListHead->lifeMonster= 0;
	
	Tile* itTile= tileListHead;
	for(int j= 0; j< alto*numPisos; j++){
		for(int i= 0; i< ancho; i++){
			if(i != 0 || j != 0){
				id++;

				Tile* aux= malloc(sizeof(Tile));
				aux->posX= i;
				aux->posY= j;
				aux->id= id;
				aux->next= NULL;
				aux->idLlave= -1;
				aux->lifeMonster= 0;
	
				
				itTile->next= aux;
				itTile= itTile->next;
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
	yEnd= yEnd+alto*pisoEnd;

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
  		itDoor->id= atoi(line);
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

  	//Impresion matriz de adyacencia con las puertas abiertas.
  	//printf("Matriz de adyacencia con puertas abiertas: \n");
  	//printMatriz(adyMatr);

  	//djikstra para matriz con puertas abiertas:
  	dijkstra(adyMatr, casillasTotal, nodoBarbaro, idEnd, distance, pred); 


  	//Cerrar puertas a matriz de adyacencia y asignar nodos a Doors.
  	itDoor= doorHeadList;
  	while(itDoor != NULL){
  		int currX1= itDoor->posX1;
		int currY1= itDoor->posY1;
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

  	//Impresion matriz de adyacencia con las puertas cerradas.
  	//printf("Matriz de adyacencia con puertas cerradas: \n");
  	//printMatriz(adyMatr);

  	//dijkstra para matriz con puertas cerradas.
  	dijkstra(adyMatr, casillasTotal, nodoBarbaro, idEnd, distance, pred);


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

void dijkstra(int** G,int n,int startnode, int finalNode, int* distance, int* pred){
	int cost[casillasTotal][casillasTotal];
	int visited[casillasTotal],count,mindistance,nextnode,i,j;
	
	//pred[] stores the predecessor of each node
	//count gives the number of nodes seen so far
	//create the cost matrix
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if(G[i][j]==0)
				cost[i][j]=INF;
			else
				cost[i][j]=G[i][j];	
	
	//initialize pred[],distance[] and visited[]
	for(i=0;i<n;i++){
		distance[i]=cost[startnode][i];
		pred[i]=startnode;
		visited[i]=0;
	}
	
	distance[startnode]=0;	
	visited[startnode]=1;
	count=1;
	
	while(count<n-1){
		mindistance=INF;
		
		//nextnode gives the node at minimum distance
		for(i=0;i<n;i++){
			if(distance[i]<mindistance&&!visited[i]){
				mindistance=distance[i];
				nextnode=i;
			}
		}
			visited[nextnode]=1;

			//check if a better path exists through nextnode			
			for(i=0;i<n;i++){
				if(!visited[i]){
					if(mindistance+cost[nextnode][i]<distance[i]){
						distance[i]=mindistance+cost[nextnode][i];
						pred[i]=nextnode;
					}
				}
			}
		count++;
	}

	if(distance[finalNode] == INF ){
		printf(" Hrognan no ha podido llegar a la salida. \n");
		return;
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


	//Imprimir el camino y distancia al nodo final
	printf("\nDistancia de Hrognan al nodo salida %d= %d.",finalNode,distance[finalNode]);
	printf("\nCamino recorrido= %d",finalNode);
	j=finalNode;
	do{
		j=pred[j];
		printf("<-%d",j);
	}while(j!=startnode);

	printf("\n");
}