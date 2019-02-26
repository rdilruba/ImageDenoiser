# ImageDenoiser

In this Project first I divide jobs as main and the algorithm part. In main processor I send the array of bits of photo and then receive the final array using MPI Send & Receive. Then in other processors I take the array from main by using MPI receive. I create a for loop to change bits of the array. In each iteration I send the recent top and bottom of my array to other processors and receive their top & bottom too. By doing this I comminicate over the processors so when they need a cell from other processors they receive it and use it to calculate the probability of change of the bit.

 In the algorithm first I choose a random x and y to change a cell then I calculate the probability of change. To calculate probability first I sum the surrounding cells of the chosen cell and then multiply it with current cell and B(beta). Then I also multiply the current cell with the original cell – it never changes it is same as it comes – and Gama. Finally I calculate the probability and again choose a random number. If this random value is less than probability that means we can change, it is probable. After changing necessary cells I send the final array to main processor by using MPI send. In main processor I combine all arrays from processors then write it to the output file.

I compile and run the code in command Shell as follows
mpicc -g 300.c -o hey -lm (lm for math lib)
mpiexec -n 6 hey input.txt output.txt 0.6 0.1
Then I check the photo by following python code

from PIL import Image
import numpy as np
from matplotlib import pyplot as plt
import imageio
from pylab import rcParams
rcParams['figure.figsize'] = 6, 6
# Reading image from the text
X = np.loadtxt("output.txt")
plt.imshow(X,cmap='gray',vmin=-1,vmax=1)
plt.show()
