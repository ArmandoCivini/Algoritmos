import heapq
from collections import deque
from grafos import Grafo
import random
import operator

def camino_minimos(padres, inicial, final):
	orden = []
	actual = final
	orden.append(actual)
	while actual != inicial:
		actual = padres[actual] 
		orden.append(actual)
	orden.reverse()	
	return orden

def g_cam_min_dij(grafo, inicial, cual_peso):
	dist = {}
	padres = {}
	for v in grafo.todos_los_v():
		dist[v] = float('inf')
	dist[inicial] = 0
	padres[inicial] = None
	heap = []
	heapq.heappush(heap, (0, inicial))
	while heap:
		v = heapq.heappop(heap)[1]
		for w in grafo.ver_ady(v):
			peso = grafo.ver_peso_a(v, w)[cual_peso]
			if dist[v] + peso  < dist[w]:
				dist[w] = dist[v] + peso 
				padres[w] = v
				heapq.heappush(heap, (dist[w], w))
	return padres, dist


def bfs(grafo, inicial):
	visitados = set()
	padres = {}
	orden = {}
	cola = deque()
	visitados.add(inicial)
	padres[inicial] = None
	orden[inicial] = 0
	cola.append(inicial)
	while cola:
		v = cola.popleft()
		for w in grafo.ver_ady(v):
			if w not in visitados:
				visitados.add(w)
				padres[w] = v
				orden[w] = orden[v] + 1
				cola.append(w)
	return padres, orden

def orden_topologico(grafo):
	grados = {}
	resul = []
	for vertice in grafo.todos_los_v():
		grados[vertice] = 0
	for v in grafo.todos_los_v():
		for w in grafo.ver_ady(v):
			grados[w] += 1
	cola = deque()
	for v in grafo.todos_los_v():
		if grados[v] == 0:
			cola.append(v)
	v = cola.popleft()
	while v:
		resul.append(v)
		for w in grafo.ver_ady(v):
			grados[w] -= 1
			if grados[w] == 0:
				cola.append(w)
		if len(cola) == 0:
			v = None
		else: v = cola.popleft()
	if len(resul) == grafo.len_grafo():
		return resul
	return None

def mts_con_prim(grafo, origen, cual_peso):
	ver = origen
	if  origen == None:	
		ver = grafo.random_v()
	visitados = set()
	visitados.add(ver)
	heap = []
	arbol = Grafo()
	for v_add in grafo.todos_los_v():
		arbol.agregar_v(v_add)
	for w in grafo.ver_ady(ver):
		heapq.heappush(heap, (grafo.ver_peso_a(ver, w)[cual_peso], ver, w))
	while heap:
		arista = heapq.heappop(heap)
		peso, v, w = arista[0], arista[1], arista[2]
		if w in visitados:
			continue
		arbol.agregar_a(v, w, peso)
		visitados.add(w)
		for u in grafo.ver_ady(w):
			if u not in visitados:
				heapq.heappush(heap, (grafo.ver_peso_a(w, u)[cual_peso], w, u))
	return arbol


def _dfs_con_cont_y_llegada(grafo, v, visitados, padres, orden, suma, n, original, visitados_nivel):
	if suma == int(n):
		if grafo.ver_conct_v(original, v):
			return True, padres, orden
		else:
			return False, None, None
	visitados_new = visitados.copy()
	visitados_new.add(v)
	for w in grafo.ver_ady(v):
		if w not in visitados_new and w not in visitados_nivel[suma]:
			padres_new = padres.copy()
			padres_new[w] = v
			orden_new = orden.copy()
			orden_new[w] = orden[v] + 1
			salio, padres_completo, orden_completo = _dfs_con_cont_y_llegada(grafo, w, visitados_new, padres_new, orden_new, suma + 1, n, original, visitados_nivel)
			if (salio):
				return True, padres_completo, orden_completo
			else:
				visitados_nivel[suma].append(w)
	return False, None, None

def  dfs_con_cont_y_llegada(grafo, inicial, n, dicc_ciuddes):
	visitados_nivel = {}
	for i in range(int(n)):
		visitados_nivel[i] = []
	visitados = set()
	for i in dicc_ciuddes[inicial]:
		padres = {}
		orden = {}
		padres[i] = None
		orden[i] = 0
		salio, padres, orden = _dfs_con_cont_y_llegada(grafo, i, visitados, padres, orden, 1, n, i, visitados_nivel)
		if salio == True:
			return salio, padres, orden, i
	return salio, None, None, None



def centralidad_intermediacion_d(grafo, cual_peso):
	cent = {}
	todos_los_v = grafo.todos_los_v()
	for v in todos_los_v:
		cent[v] = 0
	for v in todos_los_v:
		if cual_peso != None:
			padres, dist = g_cam_min_dij(grafo, v, cual_peso)
		else:
			padres, dist = bfs(grafo, v)
		cent_aux = {}
		for w in todos_los_v:
			cent_aux[w] = 0
		v_ord = sorted(dist.items(), key=operator.itemgetter(1))
		for w in v_ord:
			if padres[w[0]] == None:
				continue
			cent_aux[padres[w[0]]] += cent_aux[w[0]] + 1
		for w in todos_los_v:
			if w == v:
				continue
			cent[w] += cent_aux[w]
	return cent

def random_walk(grafo, largo, inicio, cent):
	padres = {}
	dist = {}
	act = inicio
	for i in range(largo*50):
		ady = set(grafo.ver_ady(act))
		prox = random.choice(grafo.ver_ady(act))
		cent[act] += 1
		act = prox

def centralidad_intermediacion_aprox(grafo):
	cent = {}
	todos_los_v = grafo.todos_los_v()
	for v in todos_los_v:
		cent[v] = 0
	random_walk(grafo, len(todos_los_v), v, cent)
	return cent


def _pagerank(grafo, iteraciones = 100, d = 0.85):
	todos_los_v = grafo.todos_los_v()
	pagerank = {}
	cant_de_ady = {}
	n_vertices = len(todos_los_v)
	n = 1 / n_vertices
	for v in todos_los_v:
		pagerank[v] = n
		cant_de_ady[v] = grafo.cant_de_ady(v)
	for i in range(iteraciones):
		for v in todos_los_v:
			pr_ady = 0
			for w in grafo.ver_ady(v):
				pr_ady += pagerank[w] / cant_de_ady[w]
			pagerank[v] = (1 - d)/n_vertices + d * pr_ady
	return pagerank

def tsp_no_unica_visita(grafo, origen, cual_peso, retricciones):
	todos_los_v = grafo.todos_los_v()
	M_padres_min = {}
	M_dist_minima = {}
	soluciones_posibles = []
	for v in todos_los_v:
		padres, dist = g_cam_min_dij(grafo, v, cual_peso)
		M_dist_minima[v] = dist
		M_padres_min[v] = padres
	for aeropuerto in retricciones[origen]:
		visitados = set()
		for aero in retricciones[origen]:
			visitados.add(aero)
		padres = {}
		padres[aeropuerto] = None
		tsp_backtracking(M_dist_minima, visitados, aeropuerto, 1, len(todos_los_v), 0, padres, retricciones[origen], cual_peso, soluciones_posibles, retricciones)
	solucion = min(soluciones_posibles)
	orden_sol = reconstruccion_camino(grafo, origen, solucion[1], M_padres_min, cual_peso)
	return solucion[0], orden_sol

def tsp_backtracking(m_ady, visitados, pos_act, cont, total, peso_total, padres, origen, cual_peso, sol, restricciones):
	if cont == total:
		for ori in origen:
			if ori in m_ady[pos_act]:
				padres_new = padres.copy()
				padres_new[ori] = pos_act
				sol.append((peso_total + m_ady[pos_act][origen], padres_new))
		return
	for v in m_ady:
		if v not in visitados and v in m_ady[pos_act]:
			upt_visitados = []
			for tupla in restricciones:
				vue = tupla[1]
				if v in vue:
					upt_visitados = vue
					for aereo in vue:
						visitados.add(aereo)
			padres_new = padres.copy()
			padres_new[v] = pos_act
			tsp_backtracking(m_ady, visitados, v, cont + 1, total, peso_total + (m_ady[pos_act][v]), padres_new, origen, cual_peso, sol, restricciones)
			for aereo in upt_visitados:
				visitados.pop(aereo)


def preorder(arbol, act, padres, visitados, peso_total, grafo, cual_peso):
	for v in arbol.ver_ady(act):
		if v not in visitados:
			visitados.add(act)
			if not arbol.ver_conct_v(act, v):
				padres_d, dist = g_cam_min_dij(grafo, act, cual_peso)
				peso_coneccion = dist[v]
				orden = camino_minimos(padres_d, act, v)
				for i in range(1, len(orden)):
					padres.append(orden[i])
			else:
				peso_coneccion = arbol.ver_peso_a(act, v)
				padres.append(v)
			act, peso_total = preorder(arbol, v, padres, visitados, peso_total + peso_coneccion, grafo, cual_peso)
	return act, peso_total

def mts_con_prim_con_restricciones(grafo, origen, cual_peso, rest):
	ver = origen
	if  origen == None:	
		ver = grafo.random_v()
	visitados = set()
	visitados.add(ver)
	heap = []
	arbol = Grafo()
	for v_add in grafo.todos_los_v():
		arbol.agregar_v(v_add)
	for w in grafo.ver_ady(ver):
		heapq.heappush(heap, (grafo.ver_peso_a(ver, w)[cual_peso], ver, w))
	while heap:
		arista = heapq.heappop(heap)
		peso, v, w = arista[0], arista[1], arista[2]
		if w in visitados:
			continue
		arbol.agregar_a(v, w, peso)
		visitados.add(w)
		for clave, valor in rest.items():
			if w in valor:
				for vis in valor:
					visitados.add(vis)
		for u in grafo.ver_ady(w):
			if u not in visitados:
				heapq.heappush(heap, (grafo.ver_peso_a(w, u)[cual_peso], w, u))
	return arbol

def tsp_aprox(grafo, origen, cual_peso, rest):
	orden_sol = []
	peso_t_act = float('inf')
	punto_usado = None
	padres_posta = None
	for punto in rest[origen]:
		padres = []
		arbol = mts_con_prim_con_restricciones(grafo, punto, cual_peso, rest)
		costo = 0
		visitados = set()
		visitados.add(punto)
		act, peso_total = preorder(arbol, punto, padres, visitados, costo, grafo, cual_peso)
		if not arbol.ver_conct_v(act, punto):
			padres_d, dist = g_cam_min_dij(grafo, act, cual_peso)
			peso_coneccion = dist[punto]
			orden = camino_minimos(padres_d, act, punto)
			for i in range(1, len(orden)):
				padres.append(orden[i])
		else:
			peso_coneccion = arbol.ver_peso_a(act, punto)
			padres.append(punto)
		if peso_total < peso_t_act:
			peso_t_act = peso_total
			orden_sol = padres
	return peso_t_act, orden_sol
