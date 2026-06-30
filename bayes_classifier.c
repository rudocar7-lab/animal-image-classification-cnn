#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int w, h, max;
    Pixel **pix;
} Slika;

typedef struct {
    double r, g, b;
} Vektor3;

//POMOCNA FUNKCIJA ZA PRESKAKANJE KOMENTARA I PRAZNIH REDOVA
void skip_comments(FILE *fp) {
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) continue;
        if (c == '#') {
            while ((c = fgetc(fp)) != EOF && c != '\n');
        } else {
            ungetc(c, fp);
            break;
        }
    }
}

//UCITAVANJE PPM SLIKE (P6)
Slika *ucitajPPM(const char *ime) {
    FILE *fp = fopen(ime, "rb");
    if (!fp) {
        printf("Ne mogu otvoriti %s\n", ime);
        return NULL;
    }

    char tip[3];
    if (fscanf(fp, "%2s", tip) != 1 || tip[0] != 'P' || tip[1] != '6') {
        printf("Nepodrzan format slike (treba biti P6)!\n");
        fclose(fp);
        return NULL;
    }

    skip_comments(fp);

    Slika *s = malloc(sizeof(Slika));
    if (!s) { fclose(fp); return NULL; }

    if (fscanf(fp, "%d", &s->w) != 1) { printf("Greska pri citanju sirine\n"); free(s); fclose(fp); return NULL; }
    skip_comments(fp);
    if (fscanf(fp, "%d", &s->h) != 1) { printf("Greska pri citanju visine\n"); free(s); fclose(fp); return NULL; }
    skip_comments(fp);
    if (fscanf(fp, "%d", &s->max) != 1) { printf("Greska pri citanju max vrijednosti\n"); free(s); fclose(fp); return NULL; }

    fgetc(fp);

    s->pix = malloc(s->h * sizeof(Pixel *));
    if (!s->pix) { free(s); fclose(fp); return NULL; }

    for (int i = 0; i < s->h; i++) {
        s->pix[i] = malloc(s->w * sizeof(Pixel));
        if (!s->pix[i] || fread(s->pix[i], sizeof(Pixel), s->w, fp) != (size_t)s->w) {
            printf("Greska pri citanju podataka piksela\n");
            for (int j = 0; j <= i; j++) if (s->pix[j]) free(s->pix[j]);
            free(s->pix);
            free(s);
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    return s;
}

//SMANJENJE SLIKE NA FIKSNIH 6x6 PIKSELA
Slika *smanjiNa6x6(Slika *orig) {
    int nw = 6;
    int nh = 6;

    Slika *nova = malloc(sizeof(Slika));
    nova->w = nw;
    nova->h = nh;
    nova->max = orig->max;

    nova->pix = malloc(nh * sizeof(Pixel *));
    for (int i = 0; i < nh; i++)
        nova->pix[i] = malloc(nw * sizeof(Pixel));

    // Velicina bloka za svaki piksel u 6x6 slici
    double blok_h = (double)orig->h / nh;
    double blok_w = (double)orig->w / nw;

    for (int y = 0; y < nh; y++) {
        for (int x = 0; x < nw; x++) {
            // Granice bloka u originalnoj slici
            int y_start = (int)(y * blok_h);
            int y_end = (int)((y + 1) * blok_h);
            int x_start = (int)(x * blok_w);
            int x_end = (int)((x + 1) * blok_w);

            // Osiguraj da ne prelazimo granice
            if (y_end > orig->h) y_end = orig->h;
            if (x_end > orig->w) x_end = orig->w;

            // Izracunaj prosjek svih piksela u bloku
            long sr = 0, sg = 0, sb = 0;
            int count = 0;

            for (int py = y_start; py < y_end; py++) {
                for (int px = x_start; px < x_end; px++) {
                    Pixel p = orig->pix[py][px];
                    sr += p.r;
                    sg += p.g;
                    sb += p.b;
                    count++;
                }
            }

            if (count > 0) {
                nova->pix[y][x].r = (unsigned char)(sr / count);
                nova->pix[y][x].g = (unsigned char)(sg / count);
                nova->pix[y][x].b = (unsigned char)(sb / count);
            } else {
                nova->pix[y][x].r = 0;
                nova->pix[y][x].g = 0;
                nova->pix[y][x].b = 0;
            }
        }
    }

    return nova;
}

//OSLOBADANJE MEMORIJE SLIKE
void oslobodiSliku(Slika *s) {
    if (!s) return;
    if (s->pix) {
        for (int i = 0; i < s->h; i++)
            if (s->pix[i]) free(s->pix[i]);
        free(s->pix);
    }
    free(s);
}

//PRETVARANJE SLIKE U LISTU RGB VEKTORA
Vektor3 *slikaUVektore(Slika *s, int *N) {
    *N = s->w * s->h;
    Vektor3 *arr = malloc((*N) * sizeof(Vektor3));
    int k = 0;
    for (int y = 0; y < s->h; y++) {
        for (int x = 0; x < s->w; x++) {
            arr[k].r = s->pix[y][x].r;
            arr[k].g = s->pix[y][x].g;
            arr[k].b = s->pix[y][x].b;
            k++;
        }
    }
    return arr;
}

//RACUNANJE SREDNJE VRIJEDNOSTI I VARIJANCE
void izracunajModel(Vektor3 *X, int N, Vektor3 *mean, Vektor3 *var) {
    mean->r = mean->g = mean->b = 0;
    for (int i = 0; i < N; i++) {
        mean->r += X[i].r;
        mean->g += X[i].g;
        mean->b += X[i].b;
    }
    mean->r /= N;
    mean->g /= N;
    mean->b /= N;

    var->r = var->g = var->b = 0;
    for (int i = 0; i < N; i++) {
        var->r += (X[i].r - mean->r) * (X[i].r - mean->r);
        var->g += (X[i].g - mean->g) * (X[i].g - mean->g);
        var->b += (X[i].b - mean->b) * (X[i].b - mean->b);
    }
    var->r /= N;
    var->g /= N;
    var->b /= N;

    // Minimalna varijanca da izbjegnemo dijeljenje s nulom
    if (var->r < 1) var->r = 1;
    if (var->g < 1) var->g = 1;
    if (var->b < 1) var->b = 1;
}

//BAYESOVA ODLUCIVACKA FUNKCIJA (log-likelihood)
double d_funkcija(Vektor3 x, Vektor3 m, Vektor3 v) {
    double det = v.r * v.g * v.b;
    double term1 = -0.5 * log(det);
    double term2 =
        ((x.r - m.r) * (x.r - m.r) / v.r) +
        ((x.g - m.g) * (x.g - m.g) / v.g) +
        ((x.b - m.b) * (x.b - m.b) / v.b);
    term2 *= -0.5;
    return term1 + term2;
}

//KLASIFIKACIJA SLIKE
int klasificiraj(Slika *s, Vektor3 mean_pas, Vektor3 var_pas,
                          Vektor3 mean_macka, Vektor3 var_macka) {
    int N;
    Vektor3 *X = slikaUVektore(s, &N);

    double D_pas = 0, D_macka = 0;
    for (int i = 0; i < N; i++) {
        D_pas += d_funkcija(X[i], mean_pas, var_pas);
        D_macka += d_funkcija(X[i], mean_macka, var_macka);
    }

    free(X);
    return D_pas > D_macka ? 0 : 1;  // 0 = pas, 1 = macka
}

/*UCITAVANJE SVIH SLIKA IZ FOLDERA*/
int ucitajFolder(const char *folder, Vektor3 **outX) {
    DIR *d = opendir(folder);
    if (!d) {
        printf("Ne mogu otvoriti %s\n", folder);
        return 0;
    }

    struct dirent *dir;
    int totalN = 0;
    Vektor3 *X = NULL;
    int brojSlika = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".ppm")) {
            char path[256];
            snprintf(path, sizeof(path), "%s/%s", folder, dir->d_name);

            Slika *s = ucitajPPM(path);
            if (!s) continue;

            Slika *s6x6 = smanjiNa6x6(s);

            int N;
            Vektor3 *arr = slikaUVektore(s6x6, &N);

            X = realloc(X, (totalN + N) * sizeof(Vektor3));
            for (int i = 0; i < N; i++)
                X[totalN + i] = arr[i];
            totalN += N;

            free(arr);
            oslobodiSliku(s);
            oslobodiSliku(s6x6);
            brojSlika++;
        }
    }

    closedir(d);
    *outX = X;
    printf("  Ucitano %d slika (%d vektora)\n", brojSlika, totalN);
    return totalN;
}

//MAIN PROGRAM
int main() {
    printf("Ucitavanje slika pasa...\n");
    Vektor3 *X_pas;
    int N_pas = ucitajFolder("pas", &X_pas);

    printf("Ucitavanje slika macaka...\n");
    Vektor3 *X_macka;
    int N_macka = ucitajFolder("macka", &X_macka);

    if (N_pas == 0 || N_macka == 0) {
        printf("Greska: potrebne su slike u folderima 'pas' i 'macka'\n");
        if (X_pas) free(X_pas);
        if (X_macka) free(X_macka);
        return 1;
    }

    Vektor3 mean_pas, var_pas;
    Vektor3 mean_macka, var_macka;

    printf("\nRacunanje modela...\n");
    izracunajModel(X_pas, N_pas, &mean_pas, &var_pas);
    izracunajModel(X_macka, N_macka, &mean_macka, &var_macka);

    printf("\n---- MODELI ----\n");
    printf("PAS   mean: R=%.2f G=%.2f B=%.2f\n", mean_pas.r, mean_pas.g, mean_pas.b);
    printf("PAS   var:  R=%.2f G=%.2f B=%.2f\n", var_pas.r, var_pas.g, var_pas.b);
    printf("MACKA mean: R=%.2f G=%.2f B=%.2f\n", mean_macka.r, mean_macka.g, mean_macka.b);
    printf("MACKA var:  R=%.2f G=%.2f B=%.2f\n", var_macka.r, var_macka.g, var_macka.b);

    printf("\n---- TESTIRANJE ----\n");

    DIR *d = opendir("test");
    if (!d) {
        printf("Ne mogu otvoriti test folder\n");
        free(X_pas);
        free(X_macka);
        return 1;
    }

    struct dirent *dir;
    int tocno = 0, ukupno = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".ppm")) {
            char path[256];
            snprintf(path, sizeof(path), "test/%s", dir->d_name);

            Slika *s = ucitajPPM(path);
            if (!s) continue;

            Slika *s6x6 = smanjiNa6x6(s);
            int klasa = klasificiraj(s6x6, mean_pas, var_pas, mean_macka, var_macka);

            printf("%s -> %s\n", dir->d_name, klasa == 0 ? "PAS" : "MACKA");

            oslobodiSliku(s);
            oslobodiSliku(s6x6);
            ukupno++;
        }
    }

    closedir(d);
    free(X_pas);
    free(X_macka);

    printf("\nTestirano %d slika.\n", ukupno);

    return 0;
}
