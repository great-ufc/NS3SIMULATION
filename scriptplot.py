import matplotlib.pyplot as plt 

#10
i = 0
mean=0
# line 1 points 
x1 = [] 
y1 = [] 
# plotting the line 1 points 
try:
	with open('resultados10.txt', 'r') as arquivo:				
		for linha in arquivo:
			y1.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
			mean = mean + float(linha.split(" - ")[1].split(";")[1].split(": ")[1])
			x1.append(i)
			i=i+1
except IOError:		
	pass


x1m = [] 
y1m = [] 

for count in range(30):
	x1m.append(count)
	y1m.append(mean/(i+1))




#50
i=0
mean=0
# line 2 points 
x2 = [] 
y2 = [] 
# plotting the line 2 points  
try:
	with open('resultados50.txt', 'r') as arquivo:				
		for linha in arquivo:
			y2.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
			mean = mean + float(linha.split(" - ")[1].split(";")[1].split(": ")[1])
			x2.append(i)
			i=i+1
except IOError:		
	pass

x2m = [] 
y2m = [] 

for count in range(30):
	x2m.append(count)
	y2m.append(mean/(i+1))



#100
i=0
mean=0
# line 2 points 
x3 = [] 
y3 = [] 
# plotting the line 2 points  
try:
	with open('resultados100.txt', 'r') as arquivo:				
		for linha in arquivo:
			y3.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
			mean = mean + float(linha.split(" - ")[1].split(";")[1].split(": ")[1])
			x3.append(i)
			i=i+1
except IOError:		
	pass

x3m = [] 
y3m = [] 

for count in range(30):
	x3m.append(count)
	y3m.append(mean/(i+1))



#250
i=0
mean=0
# line 2 points 
x4 = [] 
y4 = []

# plotting the line 2 points  

try:
	with open('resultados250.txt', 'r') as arquivo:				
		for linha in arquivo:
			y4.append(float(linha.split(" - ")[1].split(";")[1].split(": ")[1]))
			mean = mean + float(linha.split(" - ")[1].split(";")[1].split(": ")[1])
			x4.append(i)
			i=i+1
except IOError:		
	pass



x4m = [] 
y4m = [] 

for count in range(30):
	x4m.append(count)
	y4m.append(mean/(i+1))

fig, axs = plt.subplots(2,2)
axs[0,0].plot(x1, y1, 'r')
axs[0,0].set_title('ID 1')
#axs[0,0].plot(x1m, y1m, 'r')
axs[0,1].plot(x2, y2, 'b')
axs[0,1].set_title('ID 2')
#axs[0,1].plot(x2m, y2m, 'b') 
axs[1,0].plot(x3, y3, 'g')
axs[1,0].set_title('ID 3')
#axs[1,0].plot(x3m, y3m, 'g') 
axs[1,1].plot(x4, y4, 'y')
axs[1,1].set_title('ID 4')
#axs[1,1].plot(x4m, y4m, 'y') 

for ax in axs.flat:
    ax.set(xlabel='nº da Simulação', ylabel='Distância (m)')

# Hide x labels and tick labels for top plots and y ticks for right plots.
for ax in axs.flat:
    ax.label_outer()
  
# giving a title to my graph 
#plt.title('Distância entre a localização descoberta pelo algoritmo e a localização real do atacante') 
  
# show a legend on the plot 
#plt.legend() 
  
# function to show the plot 
plt.show() 