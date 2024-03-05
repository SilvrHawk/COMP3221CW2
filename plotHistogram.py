#
# Script to display the histogram (assumed to be 'hist.out' in the local directory).
#
import matplotlib.pyplot as plt
import string

# Extract the data from the file.
letter, count = [], []
for line in open("hist.out","r").readlines():
	l, c = [ int(str) for str in line.split() ]
	letter.append( l )
	count .append( c )

# Display
plt.bar( letter, count )

plt.xticks( range(26), ["{0}/{1}".format(c,c.upper()) for c in string.ascii_lowercase], rotation="vertical" )
plt.ylabel( "Count" )

plt.show()
