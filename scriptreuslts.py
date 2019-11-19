import statistics
from fractions import Fraction as F
from decimal import Decimal as D

#10
values=[]
try:
	with open('resultados10_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
except IOError:		
	pass
mean10 = statistics.mean(values)
std10 = statistics.pstdev(values)

#50
values=[]
try:
	with open('resultados50_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
except IOError:		
	pass
mean50 = statistics.mean(values)
std50 = statistics.pstdev(values)

#100
values=[]
try:
	with open('resultados100_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
except IOError:		
	pass
mean100 = statistics.mean(values)
std100 = statistics.pstdev(values)

#250
values=[]
try:
	with open('resultados250_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
except IOError:		
	pass
mean250 = statistics.mean(values)
std250 = statistics.pstdev(values)
print("--------Distance---------")
print(str(round(mean10*1000,2)) +" \pm "+ str(round(std10*1000,2)))
print(str(round(mean50*1000,2)) +" \pm "+ str(round(std50*1000,2)))
print(str(round(mean100*1000,2)) +" \pm "+ str(round(std100*1000,2)))
print(str(round(mean250*1000,2)) +" \pm "+ str(round(std250*1000,2)))


####################Time#############################

#10
values=[]
try:
	with open('resultados10_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[0].split(": ")[1]))
except IOError:		
	pass
mean10 = statistics.mean(values)
std10 = statistics.pstdev(values)

#50
values=[]
try:
	with open('resultados50_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[0].split(": ")[1]))
except IOError:		
	pass
mean50 = statistics.mean(values)
std50 = statistics.pstdev(values)

#100
values=[]
try:
	with open('resultados100_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[0].split(": ")[1]))
except IOError:		
	pass
mean100 = statistics.mean(values)
std100 = statistics.pstdev(values)

#250
values=[]
try:
	with open('resultados250_.txt', 'r') as arquivo:				
		for linha in arquivo:
			values.append(float(linha.split(" - ")[1].split(";")[0].split(": ")[1]))
except IOError:		
	pass
mean250 = statistics.mean(values)
std250 = statistics.pstdev(values)

print("--------Time---------")
print(str(round(mean10,2)) +" \pm "+ str(round(std10,2)))
print(str(round(mean50,2)) +" \pm "+ str(round(std50,2)))
print(str(round(mean100,2)) +" \pm "+ str(round(std100,2)))
print(str(round(mean250,2)) +" \pm "+ str(round(std250,2)))