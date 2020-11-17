import random

class Grafo:
	def __init__(self):
		self.dicc = {}
		self.cantidad = 0

	def agregar_a(self, a, b, pesos):
		if a not in self.dicc or b not in self.dicc:
			return False
		self.dicc[a][b] = pesos
		return True

	def agregar_v(self, v):
		if v in self.dicc:
			return
		self.dicc[v] = {}
		self.cantidad += 1
		return

	def ver_ady(self, v):
		ady = []
		for v_ady, p_ady in self.dicc[v].items():
			ady.append(v_ady)
		return ady

	def ver_conct_v(self, a, b):
		if b in (self.dicc[a]).keys():
			return True
		return False

	def sacar_a(self, a, b):
		self.dicc[a].pop( b, None)

	def sacar_v(self, v):
		self.dicc.pop(v, none)
		for g in self.dicc:
			self.dicc[g].pop(v, none)

	def ver_peso_a(self, a, b):
		return self.dicc[a][b]

	def todos_los_v(self):
		return self.dicc.keys()

	def len_grafo(self):
		return self.cantidad

	def random_v(self):
		return random.choice(list(self.dicc.keys()))
	
	def existe_v(self, v):
		return v in self.dicc.keys()
	
	def cant_de_ady(self, v):
		return len(self.dicc[v])
	
	def todas_las_a(self):
		aristas = []
		visitados = set()
		for v in self.dicc:
			for w in self.dicc[v]:
				arista = (v, w)
				if arista not in visitados:
					visitados.add(arista)
					visitados.add((w, v))
					aristas.append((v, w, self.dicc[v][w]))
		return aristas
