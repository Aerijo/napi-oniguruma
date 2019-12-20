export interface Capture {
  /**
   * The index the capture group starts at
   */
  start: number;

  /**
   * The index the capture group ends just before
   */
  end: number;

  /**
   * The length of the capture group
   */
  length: number;
}

export interface Match extends Capture {
  /**
   * The text inside the capture group
   */
  match: string;
}

export class OnigRegExp {
  constructor(source: string);

  searchSync(string: string, startIndex?: number): Match[] | null;
  searchCb(string: string, startIndex: number, callback: (err: Error | null, captures: Match[] | null) => void);
  searchCb(string: string, callback: (err: Error | null, captures: Match[] | null) => void);
  search(string: string, startIndex?: number): Promise<Match[] | null>;

  testSync(string: string, startIndex?: number): boolean;
  testCb(string: string, startIndex: number, callback: (err: Error | null, match: boolean) => void);
  testCb(string: string, callback: (err: Error | null, match: boolean) => void);
  test(string: string, startIndex?: number): Promise<boolean>;
}

export interface ScanResult {
  /**
   * The index of the pattern corresponding to this result
   */
  index: number;

  /**
   * An array of the capture groups for this result. They are in
   * group number order (index 0 is overall match, index 1 is
   * capture group 1, etc.)
   */
  captureIndices: Capture[];
}

export class OnigScanner {
  constructor(patterns: string[]);

  findNextMatchSync(string: string | OnigString, startIndex?: number): ScanResult | null;

  findNextMatchCb(string: string | OnigString, startIndex: number, callback: (err: Error | null, result: ScanResult | null) => void);
  findNextMatchCb(string: string | OnigString, callback: (err: Error | null, result: ScanResult | null) => void);

  findNextMatch(string: string | OnigString): Promise<ScanResult | null>;
}

export class OnigString {
  constructor(source: string);
  substring(start: number, end: number): string;
  slice(start: number, end: number): string;
  toString(): string;
  length: number;
}
