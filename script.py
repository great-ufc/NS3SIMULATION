import os
import sys

def calcditance(xar,yar,zar,xa,ya,za):
	distance = ((xar-xa)**2 + (yar - ya)**2 + (zar - za)**2)**(1/2)
	return distance

def verificatamanho():
	count = 0
	try:
		with open('Result.txt', 'r') as arquivo:				
			for linha in arquivo:
				count=count+1
				pass
	except IOError:		
		pass
	return count

def geradados(quantity, numWifi, filename):	 
	while(verificatamanho() < quantity):
		os.system("./waf --run \" scratch/mytopology -nWifi=" + str(numWifi) + " -uavmoviment=true\"")
	try:
		resultado = open(filename, "w")
		with open('Result.txt', 'r') as arquivo:
			i = 0
			for linha in arquivo:
				tempo = linha.split("--")[1].split("::")[0]
				valores = linha.split("--")[1].split("::")[1].split(";;;")
				#print(tempo)
				realattX = float(valores[0].split("->")[1].split(";;")[0])
				realattY = float(valores[0].split("->")[1].split(";;")[1])
				realattZ = float(valores[0].split("->")[1].split(";;")[2])
				aprattX = float(valores[1].split("->")[1].split(";;")[0])
				aprattY = float(valores[1].split("->")[1].split(";;")[1])
				aprattZ = float(valores[1].split("->")[1].split(";;")[2])
				distance = "Simulação " + str(i) + " - tempo: " + tempo + "; distance: " + str(calcditance(realattX, realattY, realattZ, aprattX, aprattY, aprattZ))
				print(distance)
				resultado.write(distance + '\n')
				i=i+1
				pass
			os.system("rm Result.txt")
		resultado.close()
	except IOError:
		print("A simulação não retornou nenhum resultado")


if __name__ == "__main__":
	for param in sys.argv[1:]:
		filename = "resultados"+str(param)+".txt"
		#print(param)
		geradados(30, param, filename)
	