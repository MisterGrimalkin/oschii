
double linearInterpolate(double v1, double v2, double amount) {
  return v1 + (amount * (v2 - v1));
}

double cosineInterpolate(double v1, double v2, double amount) {
  double mu = (1-cos(amount*PI))/2;
  return(v1*(1-mu)+v2*mu);
}

int intCompare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}
