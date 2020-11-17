#!/usr/bin/python3
import heapq
from collections import deque
from grafos import Grafo
import random
import os
import sys
import fun_grafos
import csv
import fileinput
import operator
POS_ARGV_PUERTOS = 1
POS_ARGV_VUELOS = 2
POS_CIUDAD = 0
POS_AERO_CODE = 1
POS_AERO_I = 0
POS_AERO_J = 1
POS_TIEMPO = 2
POS_PRECIO = 3
POS_CANT_VUELOS = 4
PESO_PRECIO = 1
PESO_TIEMPO = 0

def impresion_cent(dicc, n):
	dicc_ordenado = sorted(dicc.items(), key=operator.itemgetter(1))
	dicc_ordenado.reverse()
	final = []
	for i in range(n-1):
		print(dicc_ordenado[i][0] + ", ", end=""),
	print(dicc_ordenado[n-1][0])

def pagerank(grafo, n):
	rank = fun_grafos._pagerank(grafo)
	impresion_cent(rank, int(n))

def betweeness_centrality(grafo, cual_peso, n):
	cent = fun_grafos.centralidad_intermediacion_d(grafo, cual_peso)
	impresion_cent(cent, int(n))
	
def betweeness_centrality_aprox(grafo, n):
	cent = fun_grafos.centralidad_intermediacion_aprox(grafo)
	impresion_cent(cent, int(n))

def camino_minimos_barato_rapido(grafo, inicial, final, cual_peso, dicc_ciudades):
	padre_superior = None
	dist_a_final = float('inf')
	aeropuerto_final = None
	aero_inicial = None
	for aero in dicc_ciudades[inicial]:
		padres, dis =fun_grafos.g_cam_min_dij(grafo, aero, cual_peso)
		for ae in dicc_ciudades[final]:
			if dis[ae] < dist_a_final:
				padre_superior = padres.copy()
				dist_a_final = dis[ae]
				aeropuerto_final = ae
				aero_inicial = aero
	orden = fun_grafos.camino_minimos(padre_superior, aero_inicial, aeropuerto_final)
	for dato in range(len(orden)-1):
		print("{} -> ".format(orden[dato]), end="")
	print(orden[len(orden)-1])


def camino_minimo_escalas(grafo, inicial, final, dicc_ciudades):
	padre_superior = None
	dist_a_final = float('inf')
	aeropuerto_final = None
	aero_inicial = None
	for aero in dicc_ciudades[inicial]:
		padres, dis = fun_grafos.bfs(grafo, aero)
		for ae in dicc_ciudades[final]:
			if dis[ae] < dist_a_final:
				padre_superior = padres.copy()
				dist_a_final = dis[ae]
				aeropuerto_final = ae
				aero_inicial = aero
	orden = fun_grafos.camino_minimos(padre_superior, aero_inicial, aeropuerto_final)
	return orden


def itinerario_cultural(archivo, grafo, dicc_ciudades, dicc_aeropuertos):
	with open(archivo) as ciudades:
		linea = ciudades.readline()
		ciudades_a_visitar = linea.rstrip('\n')
		ciudades_a_visitar = ciudades_a_visitar.split(',')
		restricciones = []
		grafo_topo = Grafo()
		linea = ciudades.readline()
		while(linea):
			linea = linea.rstrip('\n')
			linea = linea.split(',')
			if not grafo_topo.existe_v(linea[0]):
				grafo_topo.agregar_v(linea[0])
			if not grafo_topo.existe_v(linea[1]):
				grafo_topo.agregar_v(linea[1])
			grafo_topo.agregar_a(linea[0], linea[1], ())
			linea = ciudades.readline() 
		orden = fun_grafos.orden_topologico(grafo_topo)
		resu = []
		resul_completo = []
		cambio_de_linea = []
		sum_ant = 0
		for i in range(len(orden)-1):
			aux = camino_minimo_escalas(grafo, orden[i], orden[i+1], dicc_ciudades)
			cambio_de_linea.append(len(aux) + sum_ant)
			sum_ant = cambio_de_linea[-1]
			for j in aux :
				if not j in resu:
					resu.append(j)
				resul_completo.append(j)
		for i in range(len(resu)-1):
			print(dicc_aeropuertos[resu[i]] + ", ", end="")
		print(dicc_aeropuertos[resu[-1]])
		cont_cambio = 0
		cambio_de_linea.pop()
		cambio_de_linea.append(0)
		for i in range(len(resul_completo)-1):
			if i == cambio_de_linea[cont_cambio]:
				cont_cambio += 1
			if i+1 == cambio_de_linea[cont_cambio]:
				print(resul_completo[i])
			else:
				print("{} -> ".format(resul_completo[i]), end="")
		print(resul_completo[-1])
		

def optimizacion_de_rutas(grafo_vuelos, ruta):
	arbol = fun_grafos.mts_con_prim(grafo_vuelos, None, PESO_PRECIO)
	aristas = arbol.todas_las_a()
	vertices = arbol.todos_los_v()
	with open(ruta, mode='w') as vuelos:
		vuelo = csv.writer(vuelos, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
		cont = 0
		for a in aristas:
			peso_a = grafo_vuelos.ver_peso_a(a[0], a[1])
			cont += peso_a[PESO_PRECIO]
			vue = [a[0], a[1], peso_a[0], 1, peso_a[2]]
			vuelo.writerow(vue)
	print("OK")


def viaje_de_N_lugares(grafo, origen, n, dicc_ciudades):
	salio, padres, orden, puerto_origen = fun_grafos.dfs_con_cont_y_llegada(grafo, origen, n, dicc_ciudades)
	if salio:
		serdap = {}
		for padre, hijo in padres.items():
			serdap[hijo] = padre
		act = puerto_origen
		cad_a_imp = ""
		while act in serdap:
			print(cad_a_imp, end="")
			cad_a_imp = act + " -> "
			ant = act
			act = serdap[act]
		print(cad_a_imp[:-4] + " -> ", end="")
		print(act + " -> ", end="")
		print(puerto_origen)
		return
	print("No se encontro recorrido")
	return

def print_para_recorrer(peso, orden):
	for i in range(len(orden)-1):
		print(orden[i] + " -> ", end='')
	print(orden[-1])
	print("costo: " + str(peso))

def recorrer_el_mundo(grafo, origen, dicc_ciudades):
	peso, orden = fun_grafos.tsp_no_unica_visita(grafo, origen, PESO_TIEMPO, dicc_ciudades)
	print_para_recorrer(peso, orden)

def recorrer_el_mundo_aprox(grafo, origen, dicc_ciudades):
	peso, orden = fun_grafos.tsp_aprox(grafo, origen, PESO_TIEMPO, dicc_ciudades)
	print_para_recorrer(peso, orden)

def main():
	if len(sys.argv) > 3 or len(sys.argv)<3:
		print("Cantidad de parametros iniciales erronea")
		return
	ciudad_aeropuertos = {}
	aeropuertos_ciudad = {}
	with open(sys.argv[POS_ARGV_PUERTOS]) as aeropuertos:
		linea = aeropuertos.readline()
		while(linea):
			linea = linea.rstrip('\n')
			linea = linea.split(',')
			if not linea[POS_CIUDAD] in ciudad_aeropuertos.keys():
				ciudad_aeropuertos[linea[POS_CIUDAD]] = [linea[POS_AERO_CODE]]
			else:
				ciudad_aeropuertos[linea[POS_CIUDAD]].append(linea[POS_AERO_CODE])
			aeropuertos_ciudad[linea[POS_AERO_CODE]] = linea[POS_CIUDAD]
			linea = aeropuertos.readline()
	with open(sys.argv[POS_ARGV_VUELOS]) as vuelos:
		grafo_vuelos = Grafo()
		linea = vuelos.readline()
		while(linea):
			linea = linea.rstrip('\n')
			linea = linea.split(',')
			if not grafo_vuelos.existe_v(linea[POS_AERO_I]):
				grafo_vuelos.agregar_v(linea[POS_AERO_I])
			if not grafo_vuelos.existe_v(linea[POS_AERO_J]):
				grafo_vuelos.agregar_v(linea[POS_AERO_J])
			grafo_vuelos.agregar_a(linea[POS_AERO_I], linea[POS_AERO_J], (int(linea[POS_TIEMPO]), int(linea[POS_PRECIO]), int(linea[POS_CANT_VUELOS])))
			grafo_vuelos.agregar_a(linea[POS_AERO_J], linea[POS_AERO_I], (int(linea[POS_TIEMPO]), int(linea[POS_PRECIO]), int(linea[POS_CANT_VUELOS])))
			linea = vuelos.readline()
	line = input()
	while line:
		try:
			line.rstrip('\n')
			line = line.split(' ', 1)
			comando = line[0]
			if(len(line)>1):
				instrucciones = line[1].split(',')
			if comando == "listar_operaciones":
				print("{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}".format("camino_mas", "camino_escalas", "centralidad", "centralidad_aprox", "pagerank", "nueva_aerolinea", "recorrer_mundo_aprox", "vacaciones", "itinerario"))
			if comando == "camino_mas":
				if instrucciones[0] == "barato":
					camino_minimos_barato_rapido(grafo_vuelos, instrucciones[1], instrucciones[2], PESO_PRECIO, ciudad_aeropuertos)
				if instrucciones[0] == "rapido":
					camino_minimos_barato_rapido(grafo_vuelos, instrucciones[1], instrucciones[2], PESO_TIEMPO, ciudad_aeropuertos)
			elif comando == "camino_escalas":
				orden = camino_minimo_escalas(grafo_vuelos, instrucciones[0], instrucciones[1], ciudad_aeropuertos)
				for dato in range(len(orden)-1):
					print(orden[dato]+" -> ", end = "")
				print(orden[len(orden)-1])
			elif comando == "centralidad":
				betweeness_centrality(grafo_vuelos, None, instrucciones[0])
			elif comando == "centralidad_aprox":
				betweeness_centrality_aprox(grafo_vuelos, instrucciones[0])
			elif comando == "pagerank":
				pagerank(grafo_vuelos, instrucciones[0])
			elif comando == "nueva_aerolinea":
				optimizacion_de_rutas(grafo_vuelos, instrucciones[0])
			elif comando == "recorrer_mundo":
				recorrer_el_mundo(grafo_vuelos, instrucciones[0], ciudad_aeropuertos)
			elif comando == "recorrer_mundo_aprox":
				recorrer_el_mundo_aprox(grafo_vuelos, instrucciones[0], ciudad_aeropuertos)
			elif comando == "vacaciones":
				viaje_de_N_lugares(grafo_vuelos, instrucciones[0], instrucciones[1], ciudad_aeropuertos)
			elif comando == "itinerario":
				itinerario_cultural(instrucciones[0], grafo_vuelos, ciudad_aeropuertos, aeropuertos_ciudad)
			line = input()
		except EOFError:
			break

main()

