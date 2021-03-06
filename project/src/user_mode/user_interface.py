# /usr/bin/python

import sys
import os

is_debug_mode = False
max_firewall_rules_num = 50

def main():
    """
        main checks the cli arguments valid 
        print error message if not
        and if they are valid
        cleanup if the argument is 0 
        else show 
    """
    try: # try execute the main
        args = sys.argv
        check_valid_input(args)

        if len(args) == 2: # the case that the cli is: <program name> 0
            if args[1] == "show_rules":
                show_rules();
            elif args[1] == "show_log":
                show_log()
            else: # args[1] == clear_log
                clear_log()
        else: # the case that the cli is: load_rules <path_to_rules_file> 
            load_rules(args)

    except FileNotFoundError as file_missing_e:
        print(file_missing_e)
    except ValueError as val_e:
        print(val_e)
    except RuntimeError as e:
        print(e)


def clear_log():
    """
        cleanup the log file
    """
    with open("/dev/fw_log", "w") as fd: # TODO: change path
        fd.truncate()
        # fd.write("0\n0\n")  // maybe need to add some sign of clear log


def show_log():
    """
        show the log file
    """
    with open("/dev/fw_log") as fd: # TODO: change path
        log_file_as_str = fd.read()
        if is_debug_mode:
            print_title()
        print(log_file_as_str)
 
def print_title():
    print("src_ip", end=" ")
    print("dst_ip", end=" ")
    print("src_port", end=" ")
    print("dst_port", end=" ")
    print("protocol", end=" ")
    print("action", end=" ")
    print("reason", end=" ")
    print("count", end="\n") # common case of print() but wanted be explicit

def show_rules():
    """
        show the rules file
    """
    with open("/sys/class/fw/rules/rules") as fd: # TODO: change path
        rules_file_as_str = fd.read()
        print(rules_file_as_str)


def load_rules(args):
    if is_too_much_rules():
        raise RuntimeError("Too many arguments [{0}] in cli".format(len(args)))
    
    with open() as fd_fw_new_content:
        str_rules = fd_fw_new_content.read()

        rules_file_as_str_dollar_splitted = '#'.join(str_rules.split())

        # TODO: check no #rules > max_firewall_rules_num

        with open("/sys/class/fw/rules/rules") as fd_fw_table: # TODO: change path
            fd_fw_table.write(rules_file_as_str_dollar_splitted)


def is_too_much_rules():
    with open("something.txt", "r") as fd_fw_new_content:
            if len(fd_fw_new_content.readlines() > max_firewall_rules_num):
                return True
    return False

def check_valid_input(args):
    """
        check that the arguments besides the program name are valid means it is nothing or the argument is 0
    """
    if len(args) not in {2,3}:
        raise RuntimeError("Too many arguments [{0}] in cli".format(len(args)))

    elif len(args) == 2:
        if args[1] not in {"show_rules", "show_log", "clear_log"}:
            raise ValueError("Invalid value [{0}] not in {1}".format(args[1], {"show_rules", "show_log", "clear_log"}))

    elif args[1] != "load_rules": # len(args) == 3
        raise ValueError("Invalid value [{0}] is not {1}".format(args[1], "load_rules"))

    elif not os.path.exists(args[2]):
        raise FileNotFoundError("No file in path [{0}] in the file system".format(args[2]))


# check if the file is executed directly and call to main function to work
if __name__ == "__main__":
    main()
