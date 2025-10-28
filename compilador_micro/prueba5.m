inicio
  leer(numero);

  si numero entonces
    contador := 0;
    mientras numero entonces
      repetir
        contador := contador + 1;
        escribir(contador);
      hasta contador;
      numero := numero - 1;
    fin;
  sino
    escribir(0);
  fin;

  escribir(numero);
fin