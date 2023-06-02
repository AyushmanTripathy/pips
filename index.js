function fibb(x) {
  if (x == 1) return 1;
  else if (x == 0) return 0;
  return fibb(x - 1) + fibb(x - 2);
}

console.log(fibb(30));
