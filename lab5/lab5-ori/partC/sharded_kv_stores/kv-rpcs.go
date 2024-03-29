package main

//
// Common RPC request/reply definitions
//

// Put(string, string): void
type PutArgs struct {
	Key   string
	Value string
}

type PutReply struct {
}

// Get(string): string
type GetArgs struct {
	Key string
}

type GetReply struct {
	Value string
	Error string
}

// Delete(string): void
type DeleteArgs struct {
	Key string
}

type DeleteReply struct {
}
