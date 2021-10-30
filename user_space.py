#/usr/bin/python

import sys

def main():

	try:
		args = sys.argv
		print("hello babe {0}".format(len(args)))

		# no cleanup argument
		if len(args) == 1:
			pass
		elif len(args) == 2:
			cleanup_arg = agrs[1]

			if type(cleanup_arg) != int:
				raise TypeError("Invalid type ({0}) not int".format(args[1]))			
	 		elif cleanup_arg != 0:
				raise ValueError("Invalid value ({0}) not zero".format(args[1]))
	
			else:
				cleanup_packet_status()
		else: # too many arguments on cli
			raise RuntimeError("Too many arguments ({0}) in cli not program name and optional cleanup argument".format(len(args)));
	except(Exception e):
		print(e)

def check_valid_input():
	pass

def cleanup_packets_status():
	print("In cleanup_packet_status()")

def show_packets_status():
	print("In show_packets_status()")



if __name__ == "__main__":
	main()
