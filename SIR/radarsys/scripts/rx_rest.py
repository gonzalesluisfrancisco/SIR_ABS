from bottle import route, run, request, post, response, request
from bottle import error
from json import dumps



#from bottle.ext import sqlite
import re

#Sockets
import socket
import sys

server_ip  = '192.168.1.128'
server_port = 8888
module = (server_ip,server_port)


namepattern = re.compile(r'^[a-zA-Z\d]{1,64}$')

#from bottle import Bottle
#app = Bottle()
#plugin = sqlite.Plugin(dbfile='/home/lgonzales/test_SIR/radarsys/sqlite.db')
#app.install(plugin)

import sqlite3

@route('/')
@route('/hello')
def hello():
    return "Hello World!"


@route('/', method='POST')
def CATCH_POST():
    """
    This function catch the REST message from the microcontroller
    """

    try:
        
        client_ip = request.environ.get('REMOTE_ADDR')
        print(client_ip)
        module_id = client_ip.split('.')
        print (type(module_id))
        module_id_last = module_id[3]
        print (module_id_last)
        print (type(module_id_last))
        int_module_id_last = int(module_id_last)
        print (int_module_id_last)


        print (type(int_module_id_last))
        for l in request.body:
            print (l)
        print (request.body.readlines())
        
        aux_lin = ("-")
        aux_lin *= 32
        print (aux_lin)
        try:
            data = request.json
            print(data)
        except:
            print("Error no esta en formato json")
            raise ValueError
        
        if data is None:
            print("Error data vacia")
            raise ValueError
        
        try:
            '''
            if namepattern.match(data["issue"]) is None:
                raise ValueError
            '''
            issue_content = data["issue"]
            print (issue_content)
        except:
            print("No existe campo 'issue'")
            raise ValueError
        rv = [{"id":1, "name": "Test Item 1"},{"id":2, "name": "Test Item 2"}]
        response.content_type = 'application/json'
        dumps(rv)

        test_armastatus = "0"
        test_armastatus *=128
        print(test_armastatus)
        list_test_armastatus = list(test_armastatus)
        list_test_armastatus[int(module_id_last)-1] = "1"
        test_armastatus ="".join(list_test_armastatus)
        print(list_test_armastatus)
        print(test_armastatus)


        print("------------")
        print("DB")
        print("------------")
        #rows = db.execute('SELECT * from * where configuration_ptr_id= 1').fetchone()
        #print(rows)

        con = sqlite3.connect('/home/lgonzales/test_SIR/radarsys/sqlite.db')
        cursor = con.cursor()

        ####
        comando_sql = "SELECT * from abs_absactive"
        cursor.execute(comando_sql)
        for i in cursor:
            print "id = ",i[0]
            print "conf_id = ",i[1]

        last_conf_active = i[1]
        print (type(last_conf_active))

        ####
        comando_sql = "SELECT * from abs_configurations where configuration_ptr_id="+str(last_conf_active)
        #cursor.execute("SELECT * from abs_configurations where configuration_ptr_id=1")
        cursor.execute(comando_sql)
        for i in cursor:
            print "id = ",i[0]
            print "active_beam = ",i[1]
            print "operation_mode = ",i[2]
            print "operation_value = ",i[3]
            #print "module_message = ",i[4]
            print "module_status = ",i[5]
            
        aux="0"*64
        list_test_armastatus = list(aux)
        print(i[5])
        print(list_test_armastatus)
        list_test_armastatus[int(module_id_last)-1] = "2"
        test_armastatus ="".join(list_test_armastatus)
        print(test_armastatus)

        ###########################################
        
        print("UPDATING")
        comando_sql = 'UPDATE abs_configurations set module_status="'+ test_armastatus+'" where configuration_ptr_id='+str(last_conf_active)
        #cursor.execute("SELECT * from abs_configurations where configuration_ptr_id=1")
        print(comando_sql)
        
        cursor.execute(comando_sql)
        
        con.commit()
        print ("Total registros actualizados: ", con.total_changes)
        
        
        comando_sql = "SELECT * from abs_configurations where configuration_ptr_id="+str(last_conf_active)
        #cursor.execute("SELECT * from abs_configurations where configuration_ptr_id=1")
        cursor.execute(comando_sql)
        for i in cursor:
            print "id = ",i[0]
            print "active_beam = ",i[1]
            print "operation_mode = ",i[2]
            print "operation_value = ",i[3]
            #print "module_message = ",i[4]
            print "module_status = ",i[5]
            
        
        con.close()
    except:
        con.close()
        return {"status":0, "message": "Error catching"}
    
    
    return {"status":1, "message": "Success catching post"}

@error(404)
def error404(error):
    return "^^' Nothing here, try again."

run(host=server_ip, port=server_port, debug=True)
