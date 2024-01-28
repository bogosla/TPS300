from configparser import ConfigParser
import codecs
import os
import subprocess


INI_FILE = "confs.ini"




class GenTPS(object):
    """Generate and Compile for TPS300

    Args:
        object (ini_file): The file Configuration
    """

    SETUP_SECTION = "setup"
    TCC_SECTION = "tcc"
    DEFS_SECTION = "defs"

    def __init__(self, ini_file) -> None:
        self.ini_file = ini_file
        self._set_config_parser()
        self._set_globals()


    def _set_config_parser(self, encoding="utf-8"):
        """Read file configuration

        Args:
            encoding (str, optional): The encode mode. Defaults to "utf-8".
        """
        self._parser = ConfigParser()
        with codecs.open(self.ini_file, mode="r", encoding=encoding) as _f:
            self._parser.read_file(_f)


    def _set_globals(self):
        """Set config variables
        """
        self._MODE = self._parser.get(GenTPS.SETUP_SECTION, "mode")
        self._CPU = self._parser.get(GenTPS.SETUP_SECTION, "cpu")
        self._TCC = self._parser.get(GenTPS.SETUP_SECTION, "tcc")
        self._ARMLINK = self._parser.get(GenTPS.SETUP_SECTION, "armlink")
        self._FROMELF = self._parser.get(GenTPS.SETUP_SECTION, "fromElf")
        self._appgen = self._parser.get(GenTPS.SETUP_SECTION, "appgen")
        self._TPLIB_PATH = self._parser.get(GenTPS.TCC_SECTION, "tplib_path")
        self._LIBPATH = self._parser.get(GenTPS.TCC_SECTION, "libpath")

        self._current_dir = os.getcwd()
        self._source_dir = os.path.join(self._current_dir, "src")
        self._object_dir = os.path.join(self._current_dir, "object")
        self._image_dir = os.path.join(self._current_dir, "image")
        self._images_dir = os.path.join(self._current_dir, "images")




    def _defs_sub(self, product_name="TPAPP", product_version="Build200404", product_desc="SDK For G50.0 Terminal", product_terminal="G50.0"):
        # TODO, Documentate
        str_defs = ""
        str_defs += '-DTP_SDK_PLATFORM\n'
        str_defs += '-DTP_CORE_840\n'
        str_defs += '-DTPS300\n'
        str_defs += '-DDUAL_SIM_CHECK\n'
        str_defs += '-DAPP_PRODUCT_NAME=\'"' + product_name + '"\'\n'
        str_defs += '-DAPP_PRODUCT_VERSION=\'"' + product_version + '"\'\n'
        str_defs += '-DAPP_PRODUCT_DESC=\'"' + product_desc + '"\'\n'
        str_defs += '-DAPP_PRODUCT_TERMINAL=\'"' + product_terminal + '"\'\n'
        return str_defs


    def generate_defs(self, path="defs.sub"):
        table_infos = ["product_name", "product_version", "product_desc", "product_terminal"]
        data = {}
        options = self._parser.options("defs")
        for tip in table_infos:
            if tip in options:
                data[tip] = self._parser.get(GenTPS.DEFS_SECTION, tip)

        with open(path, "w") as file:
            file.write(self._defs_sub(**data))


    def _tcc_sub(self):
        str_tcc = ""
        # Start generate
        str_tcc += "-I" + self._TPLIB_PATH + "\\tpcore\cfg\n"
        str_tcc += "-I" + self._TPLIB_PATH + "\\tpcore\params\n"
        str_tcc += "-I" + self._TPLIB_PATH + "\\tpcore\inc\n"
        str_tcc += "-I" + self._TPLIB_PATH + "\\tpcore\init\n"
        str_tcc += "-I" + self._source_dir + "\\\n" 

        for root, dirs, _ in os.walk(self._source_dir):
            for dir_name in dirs:
                str_tcc += "-I" + os.path.join(root, dir_name) + "\\\n"

        str_tcc += "--cpu " +  self._CPU + "\n"
        str_tcc += self._MODE + "\n"
        str_tcc += "--diag_error 187" + "\n"
        str_tcc += "-c" + "\n"
        str_tcc += "-O3" + "\n"
        str_tcc += "--apcs /interwork" + "\n"
        str_tcc += "--diag_suppress 186,236,401,1297,1301" + "\n"
        str_tcc += "--littleend" + "\n"
        str_tcc += "--EC" + "\n"

        return str_tcc


    def generate_tcc(self, path="tcc.sub"):
        with open(path, "w") as file:
            file.write(self._tcc_sub())


    def generate_objects(self, path="tcc.sub"):
        # Open the file for reading
        paths = []
        with open(path, "r") as file:
            # Read each line in the file
            for line in file:
                # Check if the line starts with "-I"
                if line.strip().startswith("-I"):
                    # Extract the path (excluding the "-I" part)
                    pathlib = line.strip()[2:]
                    paths.append(pathlib)
        
        for pathlib in reversed(paths):
            for file in os.listdir(pathlib):
                if file.endswith(".c"):
                    my_file = os.path.join(pathlib, file)
                    
                    compile_command = [self._TCC, "-o", os.path.join(self._object_dir, f"{file.lower()[:-2]}.o"), "--via",  ".\\tcc.sub", "--via", ".\defs.sub", my_file]
                    result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    
                    if result.returncode == 0:
                        print(my_file, "\033[92mOK\033[0m")
                    else:
                        print(my_file, "\033[91mfailed\033[0m")
                        print(result.stderr)

    
    # ARMLINK -o E:\TPS300\SIM800F-SDK\20200901-TPS300_800F_SDK_Release\tpbuild\object\app.elf --via app.sb0
    def generate_linking(self):
        compile_command = [self._ARMLINK, "-o", os.path.join(self._object_dir, "app.elf"), "--via", ".\\app.sb0"]
        result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        
        if result.returncode == 0:
            print("app.elf", "\033[92mOK\033[0m")
        else:
            print("app.elf", "\033[91mfailed\033[0m")
            print(result.stderr)


    def _app_sb0(self):
        str_app = ""
        
        str_app += self._TPLIB_PATH + "\\tpcore\lib\core.lib\n"
        str_app += self._TPLIB_PATH + "\\tpcore\lib\\TPS300_EN.lib\n"
        str_app += self._TPLIB_PATH + "\\tpcore\lib\\tpcharset_west.lib\n"
        str_app += "--cpu " + self._CPU + "\n"
        str_app += "--map\n"
        str_app += "--info sizes,totals\n"
        str_app += "--feedback lnkfeedback.txt\n"
        str_app += "--xref\n"
        str_app += "--noremove\n"
        str_app += "--pad 0x00\n"
        str_app += "--datacompressor off\n"
        str_app += "--list " + self._object_dir + "\\app.map\n"
        str_app += "--symdefs " + self._object_dir + "\\app.sym\n"
        str_app += "--diag_suppress 6314\n"
        str_app +=  self._TPLIB_PATH + "\\tpcore\\images\\CORE\\SIM840W64\\SIM840W64_WINBOND_EMBEDDEDAT_PCB01_gprs_MT6252_S01.sym\n"
        str_app += "--scatter " + self._images_dir + "\\APP\\SIM840W64\\scatSIM840W64_APP.txt\n"
        str_app += "--libpath " + self._LIBPATH + "\n"

        object_contents = os.listdir(self._object_dir)

        # Filter for .o files
        object_files = [item for item in object_contents if item.endswith('.o')]

        for obj_file in object_files:
            str_app += self._object_dir + "\\" + obj_file + "\n"
        return str_app
   

    def generate_sb0(self, path="app.sb0"):
        with open(path, "w") as _f:
            _f.write(self._app_sb0())


    # fromElf E:\TPS300\SIM800F-SDK\20200901-TPS300_800F_SDK_Release\tpbuild\object\app.elf  --bin --output E:\TPS300\SIM800F-SDK\20200901-TPS300_800F_SDK_Release\tpbuild\object\app
    def generate_app(self):
        compile_command = [self._FROMELF, os.path.join(self._object_dir, "app.elf"), "--bin", "--output", os.path.join(self._object_dir, "APP")]
        result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        
        if result.returncode == 0:
            print("generated APP file", "\033[92mOK\033[0m")
        else:
            print("failed to generate APP file", "\033[91mfailed\033[0m")
            print(result.stderr)


    def start(self):
        GenTPS.clear_screen()
        print("\033[95mCompile with GENTPS (bogosla)\033[0m\n")

        print("GENERATING 'tcc.sub' and 'defs.sub'")
        self.generate_tcc()
        self.generate_defs()


        GenTPS.delete_files(self._object_dir)
        
        print("\nGENERATING 'object files'")
        self.generate_objects()

        print("\nGENERATING 'app.sb0'")
        self.generate_sb0()

        print("\nLINKING 'app.elf'")
        self.generate_linking()

        print("\nGENERATING 'APP'")
        self.generate_app()


        self.generate_bin()

        print("Finished generating.\n")

    # Replace
    # appgen .\object\app 32
    # copy .\object\app .\image\app
    # By 
    # copy object\app images\APP\SIM840W64\
    # GenerateUpgradeBin_V1.1.exe > nul
    def generate_bin(self):
        compile_command = [
            "copy", 
            os.path.join(self._object_dir, "APP"), 
            os.path.join(self._images_dir, os.path.join("APP", "SIM840W64"))
        ]
        result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

        if result.returncode == 0:
            print(" ".join(compile_command))
        else:
            print(result.stderr)

        compile_command = [
            os.path.join(self._images_dir, os.path.join("APP", "SIM840W64", "GenerateUpgradeBin_V1.1")),
            "."
        ]

        result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, cwd=os.path.join(self._images_dir, os.path.join("APP", "SIM840W64")), check=True)
        captured_output = result.stdout
        print("Output:", captured_output)
        
        if result.returncode == 0:
            print(" ".join(compile_command))
        else:
            print(result.stderr)


    @classmethod
    def clear_screen(cls):
        if os.name == 'posix':  # Unix/Linux/Mac
            os.system('clear')
        elif os.name == 'nt':  # Windows
            os.system('cls')
    

    @classmethod
    def delete_files(cls, path):
        try:
            files = os.listdir(path)
            for file in files:
                file_path = os.path.join(path, file)
                if os.path.isfile(file_path):
                    os.remove(file_path)
        except OSError:
            pass








pos = GenTPS(INI_FILE)


pos.start()


