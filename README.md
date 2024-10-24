**Problemas Conhecidos**
- size must match the true size of dta pointer when calling build_packet. Otherwise, it will cause segfault( eg. data= NULL but size =1)


- Tamanho minimo do pacote. 14 bytes
- Timeout por threads
