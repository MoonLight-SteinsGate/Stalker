import os, string, cmd, sys

# Types of branch instructions
NONBRANCH = 0
DIR_BRANCH = 1
DIR_CON_BRANCH = 2
INDIR_BRANCH = 3
INDIR_CON_BRANCH = 4

REGISTER = 0x000000
NONLABEL = 0xFFFFFF

branch_ins = ["br", "bl"]
conditional_branch_ins = ["cbz", "cbnz", "tbz", "tbnz"]

filtered_address = []
target_address = []
basic_block_address = []
main_entry_addr = 0x0
text_start_addr = 0x0
text_end_addr = 0x0

def get_basic_info(target_file):
    cmd_for_main = "objdump -x %s | grep \"start address\"" % target_file
    main_entry_addr = int(os.popen(cmd_for_main).read().split(" ")[2].strip(), 16)
    cmd_for_text = "objdump -h %s | grep \".text\"" % target_file
    result_cmd_for_text = [x.strip() for x in os.popen(cmd_for_text).read().split("  ") if x != '']
    text_start_addr = int(result_cmd_for_text[2], 16)
    text_end_addr = text_start_addr + int(result_cmd_for_text[1], 16)
    return main_entry_addr, text_start_addr, text_end_addr

def analysis_instruction(line):
    ins_type = NONBRANCH
    address = int(line.split(":")[0], 16)
    if "\t" not in line[20:-1]:
        label = NONLABEL
        return address, label, ins_type
    op_ins = line[20:].split("\t")[0]
    op_num = line[20:].split("\t")[1]
    op_length = len(op_ins)
    if op_length == 1:
        #This is B instruction
        label = int(op_num.split(" ")[0], 16)
        ins_type = DIR_BRANCH
    elif op_ins[0:2] == "b." or op_ins in conditional_branch_ins:
        #This is conditional branch, such as B.cond, CBZ, CBNZ, TBZ, and TBNZ
        ins_type = DIR_CON_BRANCH
        if op_ins[0:2] == "b.":
            label = int(op_num.split(" ")[0], 16)
        elif op_ins[0:2] == "cb":
            label = int(op_num.split(" ")[1], 16)
        else:
            label = int(op_num.split(" ")[2], 16)
    elif op_ins[0:2] in branch_ins and op_ins != "brk":
        #This is branch instruction, such as BR, BRAA, BRAAZ, BRAB, BRABZ, BL, BLR, BLRAA, BLRAAZ, BLRAB, and BLRABZ
        if op_ins == "bl":
            label = int(op_num.split(" ")[0], 16)
            ins_type = DIR_BRANCH
        else:
            ins_type = INDIR_BRANCH
            label = REGISTER
    else:
        #This is not branch or conditional branch instruction
        label = NONLABEL
    
    return address, label, ins_type

def analysis_binary(filename):
    basic_block_address_list = []
    target_address_list = []
    filtered_address_list = []

    fl = open(filename)
    for line in fl.readlines():
        if line.startswith("  "):
            address, label, ins_type = analysis_instruction(line)
            if ins_type == NONBRANCH:
                filtered_address_list.append(address)
                continue
            elif ins_type == DIR_BRANCH:
                filtered_address_list.append(address)
                target_address_list.append(label)
                basic_block_address_list.append(label)
                basic_block_address_list.append(address + 4)
            elif ins_type == DIR_CON_BRANCH:
                target_address_list.append(label)
                basic_block_address_list.append(label)
                filtered_address_list.append(address)
                filtered_address_list.append(address + 4)
            elif ins_type == INDIR_BRANCH:
                filtered_address_list.append(address)
                basic_block_address_list.append(address + 4)
            else:
                filtered_address_list.append(address + 4)
        
    filtered_address = set(filtered_address_list)
    target_address = set(target_address_list)
    basic_block_address = set(basic_block_address_list)

    filtered_address = filtered_address - target_address
    filtered_address = filtered_address - basic_block_address

    return filtered_address, target_address, basic_block_address

def write_to_file(filename):
    # global filtered_address, target_address, basic_block_address, main_entry_addr, text_start_addr, text_end_addr
    global main_entry_addr, text_start_addr, text_end_addr
    fl = open(filename, "w")
    fl.truncate(0)
    fl.write(str(hex(main_entry_addr)) + "\n")
    fl.write(str(hex(text_start_addr)) + "\n")
    fl.write(str(hex(text_end_addr)) + "\n")
    # fl.write("3 " + str(hex(main_entry_addr)) + "\n")
    # fl.write("3 " + str(hex(text_start_addr)) + "\n")
    # fl.write("3 " + str(hex(text_end_addr)) + "\n")
    # for address in filtered_address:
    #     fl.write("0 " + str(hex(address)) + "\n")

if __name__ == "__main__":
    target_binary = str(sys.argv[1])
    out_dir = str(sys.argv[2])
    main_entry_addr, text_start_addr, text_end_addr = get_basic_info(target_binary)
    # os.system("objdump -d %s > %s/assemble" % (target_binary, out_dir))
    # filtered_address, target_address, basic_block_address = analysis_binary("%s/assemble" % out_dir)
    write_to_file("%s/address_infomation" % out_dir)