import os

n=10
for i in range(5):
	for j in range(1,8):
		print()
		print('w'+str(2**i)+' t'+str(j))
		print()
		os.system('./bankdriver -w'+str(2**i)+' -t'+str(j)+' -r')
		#os.system('gdb bankdriver -ex \'run -w'+str(2**i)+' -t'+str(j)+' -r\' -ex \'quit\'')