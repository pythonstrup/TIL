line_counter = 0
data_header = []
employee = []
customer_USA_only_list = []
customer = None

with open ("csv/customers.csv", "r") as customer_data:
    while True:
        data = customer_data.readline()
        if not data: break

        if line_counter==0:
            data_header = data.split(",")
        else:
            customer = data.split(",")
            if customer[10].upper() == "USA": #customer 데이터의 offset 10번째 값
                customer_USA_only_list.append(customer) #즉 country 필드가 “USA” 것만

        line_counter+=1 #sutomer_USA_only_list에 저장

print ("Header :\t", data_header)
for i in range(0,10):
    print ("Data :\t\t",customer_USA_only_list[i])
    print (len(customer_USA_only_list))
with open ("csv/customers_USA_only.csv", "w") as customer_USA_only_csv:
    for customer in customer_USA_only_list:
        customer_USA_only_csv.write(",".join(customer).strip('\n')+"\n")