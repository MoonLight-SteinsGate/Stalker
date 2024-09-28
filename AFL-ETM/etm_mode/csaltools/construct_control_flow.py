base='0x0'
prefix='0xfffff'
contextIds={'0x12FF'}
f = open("difffile")
lines_dis = f.readlines() 
f.close()
f = open("tracedata")
lines_tracedata = f.readlines()
f.close()
lines=[]

lines_etm = []
flag=False
flag2=False
Timestamp_before_context =''
for line in lines_tracedata:
	if(line.find('Context - Context ID ')==0):
		id=line[line.find('0'):line.find(',')]
		if(id in contextIds):
			flag=True
			if(flag2):
				lines_etm.append(Timestamp_before_context)
				flag2=False
		else:
			flag=False
	if(line.find('Timestamp - ')==0):
		if(flag):
			flag2=False
		else:
			Timestamp_before_context = line
			flag2=True

	if(flag):
		lines_etm.append(line)


def find_line(address):
	left=0
	right=len(lines)-1
	while(left<=right):
		mid=(left+right)//2
		if(int(lines[mid].split(':')[0],16)==address):
			return mid
		if(int(lines[mid].split(':')[0],16)<address):
			left=mid+1
		if(int(lines[mid].split(':')[0],16)>address):
			right=mid-1
	return -1

fun=[]
line_fun=[]
flag=False
next_addr=dict()
for line in lines_dis:
	if(line.find('>:')>0):
		fun.append(line[line.find('<'):line.find('>')+1])
		line_fun.append(len(lines))
		flag=True
		continue
	if(line=='\n' or line.find('...')>0):
		flag=False
	if(flag):
		lines.append(line.lstrip(' '))

def find_func(address):
	line=find_line(address)
	if(line<0):
		return False
	for i in range(len(line_fun)-1):
		if line_fun[i+1]>line:
			if(fun[i].find('plt')>0):
				return False
			return fun[i]
	if(fun[-1].find('plt')>0):
		return False
	return fun[-1]

def get_next_addr(id,n):
	flag=True
	for i in range(n+1,len(lines_etm)):
		line=lines_etm[i]
		if(flag and line.find('Address')==0):
			return line[line.find('0'):line.find(',')].lstrip('0x')
		elif(line.find('Context - Context ID ')==0):
			flag=False
			id2=line[line.find('0'):line.find(',')]
			if(id==id2):
				flag=True
	return False

def get_next_addr_valid(id,n):
	flag=True
	for i in range(n+1,len(lines_etm)):
		line=lines_etm[i]
		if(flag and line.find('Address')==0):
			addr=int(line[line.find('0'):line.find(',')].lstrip('0x'),16)
			if(find_func(addr)!=False):
				return addr
		elif(line.find('Context - Context ID ')==0):
			flag=False
			id2=line[line.find('0'):line.find(',')]
			if(id==id2):
				flag=True
	return False

def is_point(id,n):
	flag=False
	for i in range(n+1,len(lines_etm)):
		line=lines_etm[i]
		if(line.find('Address')==0):
			if(flag==True):
				addr2=line[line.find('0'):line.find(',')]
				if(addr2.find(prefix)==0):
					return addr.lstrip('0x')
				return False
			addr=line[line.find('0'):line.find(',')]
			flag=True
		elif(line.find('Context - Context ID ')==0):
			id2=line[line.find('0'):line.find(',')]
			if(id2!=id):
				return False
	return False

atom_timestamps=dict()

write_timestamps=dict()


def collect_atom(id,n):
	atom_timestamps.clear()
	atom_index=0
	atom_timestamps[atom_index]=[]
	flag=False
	for i in range(n+1,len(lines_etm)):
		line=lines_etm[i]
		if(line.find('Timestamp - ')==0):
			atom_timestamps[atom_index].append(i)
		elif(line.find('ATOM')==0):
			atom_index+=1
			atom_timestamps[atom_index]=[]
		elif(line.find('Context - Context ID ')==0 or line.find('Address')==0):
			return
	return

f=open("controlflow.txt","w+")
flag_point=False
point=False
for i in range(len(lines_etm)):
	line=lines_etm[i]
	if(line.find('Timestamp - ')==0):
		if(i not in write_timestamps):
			f.write(line)
	elif(line.find('Context - Context ID ')==0):
		f.write(line)
		id=line[line.find('0'):line.find(',')]
		next_addr[id]=int(get_next_addr(id,i),16)
	elif(line.find('Address')==0):
		addr=int(line[line.find('0'):line.find(',')],16)
		addr2=get_next_addr(id,i)
		if(point!=False):
			if(point==addr2):
				point=False
			continue
		point=is_point(id,i)
		if(find_func(addr)==False):
			continue
		line_index=find_line(addr)
		addr3=get_next_addr_valid(id,i)
		if(addr2!=False):
			next_addr[id]=int(addr2,16)
		collect_atom(id,i)
		atom_index=0
		f.write(find_func(addr))
		f.write('\n')
		while(True):
			line=lines[line_index]
			if(point!=False):
				if(int(line.split(':')[0],16)==int(point,16)):
					break
			if(line.find('//')>0):
				line=line.split('//')[0].rstrip('\t').rstrip(' ')+line[-1]

			if(line.find('	b	')>0 or line.find('	bl	')>0 or line.find('	blr	')>0 or line.find('	br	')>0 or line.find('	ret')>0):
				if(atom_timestamps.get(atom_index)!=None):
					for time in atom_timestamps[atom_index]:
						f.write(lines_etm[time])
						write_timestamps[time]=True

					atom_index+=1
			elif(line.find('	b.')>0 or line.find('	cbnz	')>0 or line.find('	cbz	')>0 or line.find('	tbnz	')>0 or line.find('	tbz	')>0):
				if(atom_timestamps.get(atom_index)!=None):
					for time in atom_timestamps[atom_index]:
						f.write(lines_etm[time])
						write_timestamps[time]=True
					atom_index+=1

			f.write(line)
			if(line.find('	b	')>0 or line.find('	bl	')>0 or line.find('	blr	')>0 or line.find('	br	')>0 or line.find('	ret')>0):
				if(line.find('	b	')<0):
					if(line.find('@plt')>0):
						addr_ret=int(line[:line.find(':')],16)+4
						flag_plt=True
						if(addr3!=False and addr_ret!=addr3):
							line_index=find_line(addr_ret)
							continue
				flag_jump=True
				break
			elif(line.find('	b.')>0 or line.find('	cbnz	')>0 or line.find('	cbz	')>0 or line.find('	tbnz	')>0 or line.find('	tbz	')>0):
				if(addr2!=False and line.find(addr2)>0):
					if(line.find('@plt')>0):
						addr_ret=int(line[:line.find(':')],16)+4
						flag_plt=True
						if(addr3!=False and addr_ret!=addr3):
							print(line)
							line_index=find_line(addr_ret)
							continue
					flag_jump=True
					break
			line_index+=1


f.close()
