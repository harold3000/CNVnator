#ifndef __HISMAKER_HH__
#define __HISMAKER_HH__

// C/C++ includes
#include <iostream> 
#include <fstream> 
#include <sstream> 
using namespace std; 

// ROOT includes
#include <TFrame.h>
#include <TKey.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TTimer.h>
#include <Getline.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TMath.h>
#include <TF1.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <Math/DistFunc.h>
#include <TPRegexp.h>
#include <THashTable.h>
#include <TGraph.h>

// Application includes
#include "AliParser.hh"
#include "Genome.hh"

// Constants
const static double PRECISION           = 0.01;
const static double GENOME_SIZE         = 2.9e+9; // Excluding gaps
const static double GENOME_CNV_FRACTION = 0.01;
const static double GENOME_SIZE_NORMAL = GENOME_SIZE*(1 - GENOME_CNV_FRACTION);
const static double GENOME_SIZE_CNV    = GENOME_SIZE*GENOME_CNV_FRACTION;
const static double CUTOFF_REGION      = 0.05;
const static double CUTOFF_TWO_REGIONS = 0.01;

class HisMaker
{
private:
  static const int N_SQRT = 100000,N_FUNC = 100000,N_INV = 10000;

private:
  int bin_size;           // Bin size
  double inv_bin_size;    // Invert bin_size
  TString root_file_name; // Name of root file with data
  TString dir_name;       // Directory in the root file
  TH1 *rd_level,*rd_level_merge,*frag_len,*dl,*dl2; // Stats on partitioning
  int chromosome_len;
  bool useMappability;
  double *sqrt_vals,*inv_vals;
  TF1 **tfuncs;
  TH1 *gen_his_signal,*gen_his_distr,*gen_his_distr_all; // His for genotyping
  double _mean,    _sigma;     // Mean and sigma of gen_his_distr
  double _mean_all,_sigma_all; // Mean and sigma of gen_his_distr_all
  TCanvas *canv_view; // Canvas for displaying
  Genome *refGenome_;
  string dir_;
  string fastafile;

public:
  HisMaker(string rootFile,Genome *genome = NULL);
  HisMaker(string rootFile,int binSize,bool useGCcorrected,
	   Genome *genome = NULL);
  ~HisMaker();

  // Input/output
private:
  void writeTreeForChromosome(string chrom,short *arr_p,short *arr_u,int len);
  bool readTreeForChromosome(TString fileName,
			     string chrom,short *arr_p,short *arr_u);
  void writeATTreeForChromosome(string chrom,int *arr,int n);
  bool writeHistograms(TH1 *his1 = NULL,TH1 *his2 = NULL,
		       TH1 *his3 = NULL,TH1 *his4 = NULL,
		       TH1 *his5 = NULL,TH1 *his6 = NULL)
  {
    return writeH(false,his1,his2,his3,his4,his5,his5);
  }
  bool writeHistogramsToBinDir(TH1 *his1 = NULL,TH1 *his2 = NULL,
			       TH1 *his3 = NULL,TH1 *his4 = NULL,
			       TH1 *his5 = NULL,TH1 *his6 = NULL)
  {
    return writeH(true,his1,his2,his3,his4,his5,his6);
  }
  bool writeH(bool useDir,
	      TH1 *his1,TH1 *his2,TH1 *his3,
	      TH1 *his4,TH1 *his5,TH1 *his6);
  void writeTreeForVcf(string chr,std::vector<int> &vcf_position,std::vector<char> &vcf_ref,
                      std::vector<char> &vcf_alt, std::vector<short> &vcf_qual,
                      std::vector<short> &vcf_nref, std::vector<short> &vcf_nalt,
                      std::vector<short> &vcf_gt, std::vector<short> &vcf_flag);
  void updateTreeIdVar(string chr,std::vector<int> &vcf_position, std::map<int,char> &vcf_refm, std::map<int,char> &vcf_altm);


public:
  TH1* getHistogram(TString name,TString alt_name = "");
  TH1* getHistogram(TString root_file,TString dir,
		    TString name,TString alt_name = "");

  // Histogram naming
private:
  TString rd_gc_name,rd_gc_xy_name;
  TString rd_gc_GC_name,rd_gc_xy_GC_name;

public:
  inline void setDataDir(string dir) { dir_ = dir; }
  inline void setFastaFile(string ff) { fastafile = ff; }
  TString getDirName(int bin);
  TString getDistrName(string chr,int bin,bool useATcoor,bool useGCcorr);
  TString getUDistrName(string chr,int bin);
  TString getRawSignalName(TString chr,int bin);
  TString getSignalName(TString chr,int bin,bool useATcoor,bool useGCcorr);
  TString getUSignalName(TString chrom,int bin);
  TString getAIBName(TString chr,int bin);
  TString getPartitionName(TString chr,int bin,bool useATcorr,bool useGCcorr);
  TString getGCName(TString chrom,int bin);
  TString getATaggrName() { return "his_at_aggr"; }
  int     readChromosome(string chrom,char *seq,int max_len);
  int     parseGCandAT(char *seq,int len,int **address,TH1 *his = NULL);

  // Making trees, histograms, parititoning, PE support
public:
  void produceTrees(string *user_chroms,int n_chroms,
		    string *user_files,int n_files,
		    bool lite);
  void addVcf(string *user_chroms,int n_chroms,
              string *user_files,int n_files,bool rmchr,bool addchr);
  void IdVar(string *user_chroms,int n_chroms,
              string *user_files,int n_files,bool rmchr,bool addchr);
  void MaskVar(string *user_chroms,int n_chroms,
              string *user_files,int n_files,bool rmchr,bool addchr);
  void mergeTrees(string *user_chroms,int n_chroms,
		  string *user_files,int n_files);
  void produceHistograms(string *chrom,int n_chroms,
			 string *root_files,int n_root_files,
			 bool useGCcorr = false);
  void produceHistogramsFa(string *chrom,int n_chroms,
       string *root_files,int n_root_files,
       bool useGCcorr = false);
  void produceHistograms_try_correct(string *user_chroms,int n_chroms);
  void produceHistogramsNew(string *user_chroms,int n_chroms);
  void aggregate(string *files,int n_files,string *chrom,int n_chroms);
  TTree *fitValley2ATbias(TH1 *his_read,TH1 *his_frg);
  void stat(string *user_chroms,int n_chroms,bool useATcorr);
  void eval(string *files,int n_files,bool useATcorr,bool useGCcorr);
  void partition(string *user_chroms,int n_chroms,
		 bool skipMasked,bool useATcorr,bool useGCcorr,
		 bool exome = false,int range = 128);
  void partition2D(string *user_chroms,int n_chroms,
		   bool skipMasked,bool useATcorr,bool useGCcorr,
		   bool exome = false,int range = 128);
  void callSVs(string *user_chroms,int n_chroms,bool useATcorr,bool useGCcorr,
	       double delta);
  void pe(string *bamss,int n_files,double over,double qual);
  void pe_for_file(string file,
		   string *bams,int n_files,double over,double qual);
private:
  double estimateDepthMaximum(string *user_chroms,int n_chroms);
  int  extract_pe(TString input,
		  string *bams,int n_bams,double over,double qual,
		  bool do_print = true,int *ses = NULL);


private:
  bool correctGC(TH1 *his,TH1 *his_gc,TH2* his_rd_gc,TH1 *his_mean);
  bool correctGCbyFragment(TH1 *his,TH1 *his_gc,TH2* his_rd_gc,TH1 *his_mean);
  void calcGCcorrection(TH2* his_rd_gc,TH1 *his_mean,double *corr,int n);
  void updateMask(double *rd,double *level,bool *mask,int n_bins,
		   double mean,double sigma);
  void updateMask_skip(double *rd,double *level,bool *mask,int n_bins,
		       double mean,double sigma);
  void calcLevels(double *level1,double *isig1,bool *mask,int n_bins,
		  int bin_band,bool skipMasked,
		  double *level2 = NULL,double *isig2 = NULL);
  bool mergeLevels(double *level,int n_bins,double delta);

public: // Viewing and genotyping
  void view(string *files,int n_files,bool useATcorr,bool useGCcorr);
  void genotype(string *files,int n_files,bool useATcorr,bool useGCcorr);

private:
  void executeROOT(TString obj_class,TString class_fun,TString args);
  void printRegion(TString chrom,int start,int end,
		   bool useATcorr,bool useGCcorr);
  void generateView(bool useATcorr,bool useGCcorr);
  void generateView(TString chrom,int start,int end,
		    bool useATcorr,bool useGCcorr,
		    string *files = NULL,int win = -1);
  void drawHistograms(TString chrom,int start,int end,int win,TString title,
		      TVirtualPad *pad,TH1 *raw,TH1 *his,TH1 *hisc,
		      TH1 *hisp,TH1* hism);
  void generateViewBAF(TString chrom,int start,int end,
                    bool useATcorr,bool useGCcorr);
  void drawHistogramsBAF(TString chrom,int start,int end,int win,TString title,
                      TVirtualPad *pad, TH1 *main, TTree *vcftreeidvar);
  bool parseInput(TString &input,
		  TString &chrom,TString &start,TString &end,TString &option);

  // Auxilary calculations
private:
  bool   isSexChrom(string chr);
  double getInverse(int n);
  double getSqrt(int n);
  TF1 *getTFunction(int n);
  void getAverageVariance(double *rd,int start,int stop,
			  double &average,double &variance,int &n);
  double testRegion(double value,double m,double s,int n);
  double testTwoRegions(double m1,double s1,int n1,double m2,double s2,int n2,
			double scale);
  double getEValue(double mean,double sigma,double *rd,int start,int end);
  bool sameLevel(double l1, double l2);
  bool getRegionLeft(double *level,int n_bins,int bin,int &start,int &stop);
  bool getRegionRight(double *level,int n_bins,int bin,int &start,int &stop);
  double getMean(TH1 *his);
  bool adjustToEValue(double mean,double sigma,double *rd,int n_bins,
		      int &start,int &end,double eval);
  int countGCpercentage(const char *seq,int low,int up);
  double gaussianEValue(double mean,double sigma,double *rd,int start,int end);
  int getChromNamesWithHis(string *names,bool useATcorr,bool useGCcorr);
  int getChromNamesWithTree(string *names,string rfn = "",bool vcf = false);
  int getChromLenWithTree(string name,string rfn = "");

public:
  void getMeanSigma(TH1 *his,double &mean,double &sigma);
};

#endif
