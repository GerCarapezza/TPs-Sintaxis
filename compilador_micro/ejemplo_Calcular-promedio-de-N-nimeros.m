inicio
  leer(cantidad);
  suma := 0;
  contador := 0;
  
  si cantidad entonces
    repetir
      leer(numero);
      suma := suma + numero;
      contador := contador + 1;
    hasta contador;
    
    escribir(suma);
  sino
    escribir(0);
  fin
fin

